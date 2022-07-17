//
// Command console system service

#define _SERVICE_NAME__    "console"


// Event handler entry point
void keyboard_event_handler(void);

// Enter function
void eventKeyboardEnter(void);

// Backspace function
void eventKeyboardBackspace(void);

// Accept a character onto the keyboard string of typed characters
void eventKeyboardAcceptChar(uint8_t new_char);



struct CommandConsoleServiceLauncher {
	
	Device keyboard_device;
	
	CommandConsoleServiceLauncher() {
		
		// Link to the keyboard device driver
		get_func_address(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), keyboard_device);
		
		task_create(_SERVICE_NAME__, sizeof(_SERVICE_NAME__), keyboard_event_handler, 100, _TASK_SERVICE__);
		
	}
}static commandConsole;
#undef _SERVICE_NAME__


void keyboard_event_handler(void) {
	
	//currentChar  = 0x00;
	uint8_t currentChar         = 0x00;
	uint8_t scanCodeAccepted    = 0x00;
	uint8_t readKeyCode         = 0x00;
	
	get_func_address(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), commandConsole.keyboard_device);
	
	// Read keyboard character
	for (uint8_t i=0; i<5; i++) {
		call_extern(commandConsole.keyboard_device, 0x00, readKeyCode);
		if (readKeyCode != 0x00) break;
	}
	
	// Check MAX string length
	if (console.keyboard_string_length < _MAX_KEYBOARD_STRING_LENGTH__) {
		currentChar = readKeyCode;
		if (readKeyCode > 0x1f) {scanCodeAccepted=1;}
	} else {
		// Allow special keys past max string length
		if (readKeyCode < 0x20) currentChar = readKeyCode;
	}
	
	// Prevent wild key repeats
	if (console.lastChar == currentChar) 
		return;
	
	console.lastChar = currentChar;
	
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
		for (uint8_t i=0; i<_DEVICE_TABLE_SIZE__; i++) {
			
			if (deviceTable.device_type[i] != _DEVICE_TYPE_MODULE__) continue;
			
			char functionName[_DEVICE_NAME_LENGTH_MAX__];
			for (uint8_t a=0; a<_DEVICE_NAME_LENGTH_MAX__; a++) functionName[a] = 0x20;
			
			// Extract name from function index
			uint8_t name_length;
			for (name_length=0; name_length < _DEVICE_NAME_LENGTH_MAX__; name_length++) {
				
				functionName[name_length] = deviceTable.device_name[i][name_length];
				if (deviceTable.device_name[i][name_length] == 0x20) break;
				
			}
			
			// Check for the function name in keyboard string
			if (string_compare(functionName, console.keyboard_string, name_length+1) == 0) continue;
			if ((console.keyboard_string[ name_length ]) != 0x20) continue;
			
			// Execute the command
			uint8_t argument=0x00;
			deviceTable.device_table[i](0x00, argument, argument, argument, argument);
			
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


