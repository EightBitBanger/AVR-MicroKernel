//
// Command console service

void keyboard_event_handler(void);
void eventKeyboardEnter(void);
void eventKeyboardBackspace(void);
void eventKeyboardAcceptChar(uint8_t new_char);

struct CommandConsoleServiceLauncher {
	CommandConsoleServiceLauncher() {
		task_create("console", 8, keyboard_event_handler, _TASK_PRIORITY_NORMAL__, _TASK_SERVICE__);
	}
}static commandConsoleServiceLauncher;

void keyboard_event_handler(void) {
	
	uint8_t scanCodeAccepted = 0;
	char currentChar=0x00;
	
	uint8_t readKeyCode=0;
	
	// Link to the keyboard device driver
	Device device;
	get_func_address(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), device);
	
	// Read keyboard character
	call_extern(device, 0x00, readKeyCode);
	currentChar = readKeyCode;
	
	// Check MAX string length
	if (console.keyboard_string_length < _MAX_KEYBOARD_STRING_LENGTH__) {
		// ASCII char
		currentChar = readKeyCode;
		if (readKeyCode > 0x1f) {scanCodeAccepted=1;}
	} else {
		// Allow special keys past max string length
		if (readKeyCode < 0x20) currentChar = readKeyCode;
	}
	
	// Prevent wild key repeats
	if (console.lastChar == currentChar) {console.lastChar = currentChar; return;} console.lastChar = currentChar;
	
	// Check special keys
	switch (currentChar) {
		
		case 0x01: eventKeyboardBackspace(); break;
		case 0x02: eventKeyboardEnter(); break;
		
		default: break;
	}
	
	// Check key accepted
	if (scanCodeAccepted == 1)
	eventKeyboardAcceptChar(currentChar);
	
	return;
}

void eventKeyboardEnter(void) {
	
	console.last_string_length = console.keyboard_string_length+1;
	console.keyboard_string_length = 0;
	
	if (console.cursorLine == 3) {console.shiftUp();}
	if (console.cursorLine == 2) console.cursorLine++;
	if (console.cursorLine == 1) console.cursorLine++;
	if (console.cursorLine == 0) {if (console.promptState == 0) {console.promptState++;} else {console.cursorLine++;}}
	
	// Execute the function
	if (console.last_string_length > 0) {console.cursorPos=0;
		
		// Function look up
		for (uint8_t i=0; i<_COMMAND_TABLE_SIZE__; i++) {
			
			if (moduleTable.functionNameIndex[i][0] == 0x20) continue;
			
			char functionName[_COMMAND_NAME_LENGTH_MAX__];
			for (uint8_t a=0; a<_COMMAND_NAME_LENGTH_MAX__; a++) functionName[a] = 0x20;
			
			// Extract function name
			uint8_t name_length;
			for (name_length=0; name_length < _COMMAND_NAME_LENGTH_MAX__; name_length++) {
				
				if (moduleTable.functionNameIndex[i][name_length] == 0x20) break;
				
				functionName[name_length] = moduleTable.functionNameIndex[i][name_length];
			}
			
			// Check for the function name in keyboard string
			if (string_compare(functionName, console.keyboard_string, name_length + 1) == 0) continue;
			
			// Execute the command
			moduleTable.command_function_table[i]();
			
			break;
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
			_delay_ms(80);
			} else {
			console.cursorLine++;
		}
		console.cursorPos=0;
		
		console.setCursorPosition(console.cursorLine, console.cursorPos);
	}
	
	return;
}


