//
// Command console interface

#define _MAX_KEYBOARD_STRING_LENGTH__  32

struct CommandConsole {
	
	char  promptString[8];
	uint8_t promptStringLength;
	uint8_t promptState;
	uint8_t cursorLine;
	uint8_t cursorPos;
	
	uint8_t keyboard_string_length;
	char  keyboard_string[_MAX_KEYBOARD_STRING_LENGTH__];
	uint8_t keyboardState;
	uint8_t lastChar;
	uint8_t last_string_length;
	
	// Display driver entry pointer
	EntryPtr displayDriverPtr;
	
	CommandConsole() {
		
		for (uint8_t i=0; i<promptStringLength; i++) promptString[i] = 0x20;
		promptString[0]      = '-';
		promptStringLength   = 1;
		
		promptState     = 1;
		cursorLine      = 0;
		cursorPos       = 0;
		
		keyboard_string_length = 0;
		clearString();
		
		keyboardState=0;
		lastChar=0;
		last_string_length=0;
		
	}
	
	void initiate(void) {
		
		// Initiate current keyboard character
		EntryPtr keyboardPtr = getFuncAddress(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__));
		callExtern(keyboardPtr, 0x00, lastChar);
		
		// Link the display driver
		displayDriverPtr = getFuncAddress(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__));
		callExtern(displayDriverPtr, 0x05);
		
	}
	
	// Clear the display buffer
	void clearBuffer(void) {callExtern(displayDriverPtr, 0x04); return;}
	// Clear the mask buffer
	void clearMask(void) {callExtern(displayDriverPtr, 0x05); return;}
	
	// Clear the keyboard string
	void clearString(void) {for (uint8_t i=0; i <= 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
	
	// Sets the cursor position
	void setCursorPosition(uint8_t Line, uint8_t Pos) {
		cursorLine = Line;
		cursorPos  = Pos;
		callExtern(displayDriverPtr, 0x00, Line, Pos);
		return;
	}
	
	// Shift the display up one line
	void shiftUp(void) {callExtern(displayDriverPtr, 0x07); _delay_ms(100);}
	
	// Print chars/strings to the console
	void print(const char charArray[], uint8_t string_length) {
		for (uint8_t i=0; i<string_length-1; i++) {
			callExtern(displayDriverPtr, 0x09, (uint8_t&)charArray[i], cursorLine, cursorPos);
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
	void printInt(int number) {
		
		char numberString[8];
		uint8_t place = intToString(number, numberString);
		if (place==0) place++;
		
		for (uint8_t i=0; i<place; i++) {
			callExtern(displayDriverPtr, 0x09, (uint8_t&)numberString[i], cursorLine, cursorPos);
			cursorPos++;
		}
		return;
	}
	
	// Add new blank line
	void printLn(void) {cursorPos = 0; if (cursorLine < 3) {cursorLine++;} else {shiftUp();} return;}
	
	// Display a command prompt
	void printPrompt(void) {
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


