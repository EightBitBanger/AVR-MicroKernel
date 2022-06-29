//
// Command console program
//  Requires support of display and keyboard drivers

#define _MAX_KEYBOARD_STRING_LENGTH__  32

void ConsoleLibraryEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct CommandConsole {
	
	// Prompt
	char  promptString[8];
	uint8_t promptStringLength;
	uint8_t promptState;
	uint8_t cursorLine;
	uint8_t cursorPos;
	
	// Keyboard string
	uint8_t keyboard_string_length;
	char  keyboard_string[_MAX_KEYBOARD_STRING_LENGTH__];
	uint8_t keyboardState;
	uint8_t lastChar;
	uint8_t last_string_length;
	
	Device displayDriverPtr;
	Device keyboardDriverPtr;
	
	CommandConsole() {
		
		// Load the device driver
		load_library(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), (Device)ConsoleLibraryEntryPoint, _DEVICE_TYPE_DRIVER__);
		
		for (uint8_t i=0; i<promptStringLength; i++) promptString[i] = 0x20;
		promptString[0]      = '>';
		promptStringLength   = 1;
		
		promptState     = 1;
		cursorLine      = 0;
		cursorPos       = 0;
		
		keyboard_string_length = 0;
		
		keyboardState=0;
		lastChar=0;
		last_string_length=0;
		
	}
	
	void initiate(void) {
		
		// Link to the keyboard driver
		if (get_func_address(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), keyboardDriverPtr) == 0) return;
		call_extern(keyboardDriverPtr, 0x00, lastChar);
		
		// Link to the display driver
		if (get_func_address(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), displayDriverPtr) == 0) return;
		call_extern(displayDriverPtr, 0x05); // Clear mask memory
		call_extern(displayDriverPtr, 0x04); // Clear char memory
		
		//printPrompt();
		clearKeyboardString();
		
	}
	
	// Clear the display buffer
	void clearBuffer(void) {call_extern(displayDriverPtr, 0x04); _delay_ms(80); return;}
	// Clear the mask buffer
	void clearMask(void) {call_extern(displayDriverPtr, 0x05); _delay_ms(80); return;}
	
	// Clear the keyboard string
	void clearKeyboardString(void) {for (uint8_t i=0; i <= 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
	
	void setCursorPosition(uint8_t Line, uint8_t Pos) {
		cursorLine = Line;
		cursorPos  = Pos;
		call_extern(displayDriverPtr, 0x00, Line, Pos);
		return;
	}
	
	void setCursorCharacter(uint8_t character) {call_extern(displayDriverPtr, 0x01, character); return;}
	
	void setCursorBlinkRate(uint8_t blink_rate) {call_extern(displayDriverPtr, 0x02, blink_rate); return;}
	
	void setCursorRefreshRate(uint8_t refresh_rate) {call_extern(displayDriverPtr, 0x03, refresh_rate); return;}
	
	// Shift the display up one line
	void shiftUp(void) {call_extern(displayDriverPtr, 0x07); _delay_ms(100);}
	
	void checkShiftUp(void) {
		if (cursorPos == 20) {cursorPos=0;
			if (cursorLine < 3) {cursorLine++;} else {shiftUp();}
		}
	}
	
	// Print chars/strings to the console
	void print(const char char_array[], uint8_t string_length) {
		for (uint8_t i=0; i<string_length-1; i++) {
			call_extern(displayDriverPtr, 0x09, (uint8_t&)char_array[i], cursorLine, cursorPos);
			cursorPos++;
		}
		return;
	}
	
	void printChar(char character) {
		call_extern(displayDriverPtr, 0x09, (uint8_t&)character, cursorLine, cursorPos);
		cursorPos++;
		checkShiftUp();
		return;
	}
	void printSpace(void) {
		uint8_t character=0x20;
		call_extern(displayDriverPtr, 0x09, (uint8_t&)character, cursorLine, cursorPos);
		cursorPos++;
		checkShiftUp();
		return;
	}
	void printInt(uint32_t number) {
		
		char numberString[10];
		uint8_t place = int_get_string(number, numberString);
		if (place==0) place++;
		
		for (uint8_t i=0; i<place; i++) {
			call_extern(displayDriverPtr, 0x09, (uint8_t&)numberString[i], cursorLine, cursorPos);
			cursorPos++;
			checkShiftUp();
		}
		
		call_extern(displayDriverPtr, 0x00, cursorLine, cursorPos); // Set the cursor line and position
		
		return;
	}
	
	// Print a new blank line
	void printLn(void) {
		
		cursorPos = 0;
		if (cursorLine < 3) {cursorLine++;} else {shiftUp();}
		
		call_extern(displayDriverPtr, 0x00, cursorLine, cursorPos); // Set the cursor line and position
		return;
	}
	
	// Print a command prompt
	void printPrompt(void) {
		
		// Cursor position following the prompt
		cursorPos = promptStringLength;
		uint8_t promptPos = 0;
		
		call_extern(displayDriverPtr, 0x00, cursorLine, cursorPos); // Set the cursor line and position
		
		// Draw the prompt
		for (uint8_t i=0; i < promptStringLength; i++) {
			
			if (promptString[i] != 0x20) {
				
				call_extern(displayDriverPtr, 0x09, (uint8_t&)promptString[i], cursorLine, promptPos);
				promptPos++;
			}
			
		}
		
		return;
	}
	
}static console;


// Driver entry point
void ConsoleLibraryEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DEVICE_INITIATE__: {console.initiate(); break;}
		case _DEVICE_SHUTDOWN__: {break;}
		
		case 0x00: console.printChar(paramA); break;
		case 0x01: console.printLn(); break;
		case 0x02: console.printPrompt(); break;
		case 0x03: console.printSpace(); break;
		
		case 0x04: {
			WrappedPointer pointer;
			pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
			console.printInt(pointer.address);
			break;
		}
		case 0x0a: console.setCursorPosition(paramA, paramB); break;
		case 0x0b: console.setCursorCharacter(paramA); break;
		case 0x0c: console.setCursorBlinkRate(paramA); break;
		
		default: break;
	}
	
	return;
}


