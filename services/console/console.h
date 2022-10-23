//
// Command console system service

void keyboard_event_handler(void);               // Event handler entry point
void eventKeyboardEnter(void);                   // Enter function
void eventKeyboardBackspace(void);               // Backspace function
void eventKeyboardAcceptChar(uint8_t new_char);  // Accept a character onto the keyboard string of typed characters

struct CommandConsoleServiceLauncher {
	
	Device keyboard_device;
	
	uint8_t scanCodeLow;
	uint8_t scanCodeHigh;
	
	CommandConsoleServiceLauncher() {
		
		// Link to the keyboard device driver
		keyboard_device = (Device)get_func_address(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__));
		
		// Prevent a key from being accepted on system startup
		call_extern(keyboard_device, 0x02, scanCodeLow, scanCodeHigh);
		
		call_extern(keyboard_device, 0x00, console.lastChar);
		
	}
}static commandConsole;


void keyboard_event_handler(void) {
	
	uint8_t currentChar         = 0x00;
	uint8_t scanCodeAccepted    = 0x00;
	uint8_t readKeyCode         = 0x00;
	
	uint8_t scanCodeLow         = 0x00;
	uint8_t scanCodeHigh        = 0x00;
	
	// Check for a scan code change
	call_extern(commandConsole.keyboard_device, 0x02, scanCodeLow, scanCodeHigh);
	if ((commandConsole.scanCodeLow == scanCodeLow) & (commandConsole.scanCodeHigh == scanCodeHigh))
		return;
	
	commandConsole.scanCodeLow  = scanCodeLow;
	commandConsole.scanCodeHigh = scanCodeHigh;
	
	// Read and convert scan code to an ASCII character
	call_extern(commandConsole.keyboard_device, 0x00, readKeyCode);
	
	// Check MAX string length
	if (console.keyboard_string_length < _MAX_KEYBOARD_STRING_LENGTH__) {
		currentChar = readKeyCode;
		if (readKeyCode > 0x1f) {scanCodeAccepted=1;}
	} else {
		// Allow special keys past max string length
		if (readKeyCode < 0x20) currentChar = readKeyCode;
	}
	
	if (console.lastChar == currentChar) return;
	console.lastChar = currentChar;
	
	// Check special keys
	switch (currentChar) {
		
		case 0x01: {eventKeyboardBackspace(); break;}
		case 0x02: {eventKeyboardEnter(); break;}
		case 0x11: {console.shiftState = 1; break;}
		case 0x12: {console.shiftState = 0; break;}
		
		default: break;
	}
	
	// Check key accepted
	if (scanCodeAccepted == 1) {
		
		// Check shift state
		if (console.shiftState == 1) {
			
			if ((currentChar >= 'a') & (currentChar <= 'z')) {
				currentChar -= 0x20;
			} else {
				switch (currentChar) {
					
					case '0': currentChar = ')'; break;
					case '1': currentChar = '!'; break;
					case '2': currentChar = '@'; break;
					case '3': currentChar = '#'; break;
					case '4': currentChar = '$'; break;
					case '5': currentChar = '%'; break;
					case '6': currentChar = '^'; break;
					case '7': currentChar = '&'; break;
					case '8': currentChar = '*'; break;
					case '9': currentChar = '('; break;
					
					case '[': currentChar = '{'; break;
					case ']': currentChar = '}'; break;
					case 0x5c: currentChar = '|'; break;
					case ';': currentChar = ':'; break;
					case 0x27: currentChar = 0x22; break;
					case ',': currentChar = '<'; break;
					case '.': currentChar = '>'; break;
					case '/': currentChar = '?'; break;
					case '-': currentChar = '_'; break;
					case '=': currentChar = '+'; break;
					case '`': currentChar = '~'; break;
					
					default: break;
				}
				
			}
			
		}
		
		eventKeyboardAcceptChar(currentChar);
		
	}
	
	return;
}

void eventKeyboardEnter(void) {
	
	console.last_string_length = console.keyboard_string_length + 1;
	console.keyboard_string_length = 0;
	
	if (console.cursorLine == 3) {console.shiftUp();}
	if (console.cursorLine == 2) console.cursorLine++;
	if (console.cursorLine == 1) console.cursorLine++;
	if (console.cursorLine == 0) {if (console.promptState == 0) {console.promptState++;} else {console.cursorLine++;}}
	
	// Check string length
	if (console.last_string_length > 0) {console.cursorPos=0;
		
		// Check change device focus
		if ((console.keyboard_string[1] == ':') & (console.keyboard_string[2] == 0x20)) {
			
			uint8_t new_device = console.keyboard_string[0];
			
			if ((new_device >= 'a') & (new_device <= 'e'))
			console.promptString[0] = (new_device - 0x20);
			
			console.clearKeyboardString();
			console.printPrompt();
			
			return;
		}
		
		// Function look up
		for (uint8_t i=0; i<DEVICE_TABLE_SIZE; i++) {
			
			if (device_table.type[i] != DEVICE_TYPE_MODULE) continue;
			
			char functionName[DEVICE_NAME_LENGTH_MAX];
			for (uint8_t a=0; a<DEVICE_NAME_LENGTH_MAX; a++) functionName[a] = 0x20;
			
			// Extract name from function index
			uint8_t name_length;
			for (name_length=0; name_length < DEVICE_NAME_LENGTH_MAX; name_length++) {
				
				functionName[name_length] = device_table.name[i][name_length];
				if (device_table.name[i][name_length] == 0x20) break;
				
			}
			
			// Check for the function name in keyboard string
			if (strcmp(functionName, console.keyboard_string, name_length+1) == 0) continue;
			if ((console.keyboard_string[ name_length ]) != 0x20) continue;
			
			// Execute the command
			Device console_function;
			console_function = (void(*)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&))device_table.table[i];
			console_function(0x00, nullchar, nullchar, nullchar, nullchar);
			
			console.clearKeyboardString();
			console.printPrompt();
			
			return;
		}
		
		if (console.keyboard_string[0] == 0x20) {
			console.clearKeyboardString();
			console.printPrompt();
			return;
		}
		
		// Function not found. Check if the filename exists
		if ((fs.file_open(console.keyboard_string) != 0) & (console.last_string_length > 1) & (fs.file_get_attribute(console.keyboard_string, 0) == 'x')) {
			
			// File found with executable attribute
			
			// Simple instruction set
			// 
			// add   0x01 - Add a register into a register
			// add   0x02 - Add a byte into a register
			// 
			// mov   0xa0 - Move a byte into a register
			// movr  0xa1 - Move a register into a register
			// 
			// int   0x3d - Run an interrupt routine
			// 
			// jmp   0xea - Jump unconditionally to the offset
			// je           Jump to the offset if register A is equal to B
			// jg           Jump to the offset if register A is greater than B
			// jl           Jump to the offset if register A is less than B
			// 
			// call  0x9a - Call to a function offset
			// ret          Return from a function call
			// 
			// push  0x06 - Push register A onto the stack
			// pop   0x07 - Pop data off the stack into register A
			
			// General purpose registers
			char reg[31];
			
			for (uint8_t b=0; b < 31; b++) 
				reg[b] = 0x00;
			
			char opcode=0x00;
			char operandA=0x00;
			char operandB=0x00;
			char operandC=0x00;
			char operandD=0x00;
			
			uint32_t file_start = (SECTOR_SIZE + 1);
			uint32_t file_end   = file_start + fs.file_size;
			
			for (uint32_t ip=file_start; ip <= file_end; ip++) {
				
				// Get the op-code
				fs.file_read_byte(ip, opcode);
				
				//
				// ADD - Add a byte to a register
				if (opcode == 0x01) {
					ip++; fs.file_read_byte(ip, operandA);
					ip++; fs.file_read_byte(ip, operandB);
					
					reg[operandA] += operandB;
					
					continue;
				}
				
				//
				// ADDr - Add a register to a register
				if (opcode == 0x01) {
					ip++; fs.file_read_byte(ip, operandA);
					ip++; fs.file_read_byte(ip, operandB);
					
					reg[operandA] += reg[operandB];
					
					continue;
				}
				
				//
				// MOV - Move byte into a register
				if (opcode == 0xa0) {
					ip++; fs.file_read_byte(ip, operandA);
					ip++; fs.file_read_byte(ip, operandB);
					
					reg[operandA] = operandB;
					
					continue;
				}
				
				//
				// MOVr - Move register into a register
				if (opcode == 0xa1) {
					ip++; fs.file_read_byte(ip, operandA);
					ip++; fs.file_read_byte(ip, operandB);
					
					reg[operandA] = reg[operandB];
					
					continue;
				}
				
				//
				// INT - Interrupt routine
				if ((opcode == 0x3d) | (opcode == 0xcd)) {
					ip++; fs.file_read_byte(ip, operandA);
					
					// INT 10 - Display call
					if (operandA == 0x10) {console.printChar( reg[0] ); continue;}
					
					// INT 20 - Program termination call
					if (operandA == 0x20) break;
					
					continue;
				}
				
				//
				// JMP - Jump to non-relative offset
				if (opcode == 0xea) {
					
					WrappedPointer pointer;
					
					ip++; fs.file_read_byte(ip, pointer.byte[3]);
					ip++; fs.file_read_byte(ip, pointer.byte[2]);
					ip++; fs.file_read_byte(ip, pointer.byte[1]);
					ip++; fs.file_read_byte(ip, pointer.byte[0]);
					
					ip = (file_start + pointer.address) - 1;
					
					continue;
				}
				
				//
				// Push a register onto the stack
				if (opcode == 0x06) {
					uint32_t ptr = exMem.stack_push(1);
					exMem.write(ptr, reg[ operandA ]);
					continue;
				}
				
				//
				// Pop a byte off the stack into a register
				if (opcode == 0x07) {
					uint32_t ptr = (exMem._STACK_BEGIN__ + exMem.stack_size());
					exMem.stack_pop(1);
					exMem.read(ptr, reg[ operandA ]);
					continue;
				}
				
			}
			
			console.clearKeyboardString();
			console.printLn();
			
		} else {
			if (console.last_string_length > 1) {
				console.print("Bad cmd or filename", sizeof("Bad cmd or filename"));
				console.printLn();
			}
		}
		
	}
	
	console.clearKeyboardString();
	console.printPrompt();
	return;
}

void eventKeyboardBackspace(void) {
	if (console.keyboard_string_length > 0) {
		
		if (console.cursorPos == 0) {
			
			if (console.cursorLine > 0) {
				console.cursorPos = 20;
				console.cursorLine--;
			}
			
		}
		
		// Overwrite the character on the display ...
		console.cursorPos--;
		console.printSpace();
		console.cursorPos--;
		console.setCursorPosition(console.cursorLine, console.cursorPos);
		
		// and the keyboard string
		console.keyboard_string_length--;
		console.keyboard_string[console.keyboard_string_length] = 0x20;
	}
	return;
}

void eventKeyboardAcceptChar(uint8_t new_char) {
	
	console.keyboard_string[console.keyboard_string_length] = new_char;
	console.keyboard_string_length++;
	
	// Add the ASCII char
	console.printChar((char&)new_char);
	console.setCursorPosition(console.cursorLine, console.cursorPos);
	
	// Check to increment the display line
	if (console.cursorPos == 20) {
		
		if (console.cursorLine == 3) {
			console.shiftUp();
		} else {
			console.cursorLine++;
		}
		
		console.cursorPos=0;
		
		console.setCursorPosition(console.cursorLine, console.cursorPos);
	}
	
	return;
}

