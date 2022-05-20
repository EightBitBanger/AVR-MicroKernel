//
// Command console program

#define _MAX_KEYBOARD_STRING_LENGTH__  32

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
	
	DriverEntryPoint displayDriverPtr;
	DriverEntryPoint keyboardDriverPtr;
	
	CommandConsole() {
		
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
		getFuncAddress(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), keyboardDriverPtr);
		callExtern(keyboardDriverPtr, 0x00, lastChar);
		
		// Link to the display driver
		getFuncAddress(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), displayDriverPtr);
		callExtern(displayDriverPtr, 0x05);
		callExtern(displayDriverPtr, 0x04);
		
		_delay_ms(100);
		
		printPrompt();
		clearString();
		
	}
	
	// Clear the display buffer
	void clearBuffer(void) {callExtern(displayDriverPtr, 0x04); _delay_ms(80); return;}
	// Clear the mask buffer
	void clearMask(void) {callExtern(displayDriverPtr, 0x05); _delay_ms(80); return;}
	
	// Clear the keyboard string
	void clearString(void) {for (uint8_t i=0; i <= 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
	
	void setCursorPosition(uint8_t Line, uint8_t Pos) {
		cursorLine = Line;
		cursorPos  = Pos;
		callExtern(displayDriverPtr, 0x00, Line, Pos);
		return;
	}
	
	void setCursorCharacter(uint8_t character) {callExtern(displayDriverPtr, 0x01, character); return;}
	
	void setCursorBlinkRate(uint8_t blink_rate) {callExtern(displayDriverPtr, 0x02, blink_rate); return;}
	
	void setCursorRefreshRate(uint8_t refresh_rate) {callExtern(displayDriverPtr, 0x03, refresh_rate); return;}
	
	// Shift the display up one line
	void shiftUp(void) {callExtern(displayDriverPtr, 0x07); _delay_ms(100);}
	
	// Print chars/strings to the console
	void print(const char char_array[], uint8_t string_length) {
		for (uint8_t i=0; i<string_length-1; i++) {
			callExtern(displayDriverPtr, 0x09, (uint8_t&)char_array[i], cursorLine, cursorPos);
			cursorPos++;
		}
		return;
	}
	void printChar(char character) {
		callExtern(displayDriverPtr, 0x09, (uint8_t&)character, cursorLine, cursorPos);
		cursorPos++;
		return;
	}
	void printSpace(void) {
		uint8_t character=0x20;
		callExtern(displayDriverPtr, 0x09, (uint8_t&)character, cursorLine, cursorPos);
		cursorPos++;
		return;
	}
	void printInt(uint32_t number) {
		
		char numberString[10];
		uint8_t place = intToString(number, numberString);
		if (place==0) place++;
		
		for (uint8_t i=0; i<place; i++) {
			callExtern(displayDriverPtr, 0x09, (uint8_t&)numberString[i], cursorLine, cursorPos);
			cursorPos++;
		}
		
		callExtern(displayDriverPtr, 0x00, cursorLine, cursorPos); // Set the cursor line and position
		
		return;
	}
	
	// Print a new blank line
	void printLn(void) {
		
		cursorPos = 0;
		if (cursorLine < 3) {cursorLine++;} else {shiftUp();}
		
		callExtern(displayDriverPtr, 0x00, cursorLine, cursorPos); // Set the cursor line and position
		return;
	}
	
	// Print a command prompt
	void printPrompt(void) {
		
		// Cursor following the prompt
		cursorPos = promptStringLength;
		uint8_t promptPos = 0;
		
		callExtern(displayDriverPtr, 0x00, cursorLine, cursorPos); // Set the cursor line and position
		
		// Draw the prompt
		for (uint8_t i=0; i < promptStringLength; i++) {
			
			if (promptString[i] != 0x20) {
				
				callExtern(displayDriverPtr, 0x09, (uint8_t&)promptString[i], cursorLine, promptPos);
				promptPos++;
			}
			
		}
		
		return;
	}
	
};
CommandConsole console;




void ConsoleLibraryEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ConsoleLibraryLoader {
	ConsoleLibraryLoader() {loadLibrary(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), (DriverEntryPoint)ConsoleLibraryEntryPoint);}
};ConsoleLibraryLoader consoleLibraryLoader;

void ConsoleLibraryEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DRIVER_INITIATE__: {console.initiate(); break;}
		case _DRIVER_SHUTDOWN__: {break;}
		
		case 0x00: console.printChar(paramA); break;
		case 0x01: console.printLn(); break;
		case 0x02: console.printPrompt(); break;
		case 0x03: console.printSpace(); break;
		
		default: break;
	}
	
	return;
}


