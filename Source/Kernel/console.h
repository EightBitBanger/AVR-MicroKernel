//
// Command console interface

struct CommandConsole {
	
	char  promptCharacter;
	uint8_t promptState;
	uint8_t cursorLine;
	uint8_t cursorPos;
	
	uint8_t keyboard_string_length;
	char  keyboard_string[24];
	uint8_t keyboardState;
	uint8_t lastChar;
	
	// Display driver entry pointer
	EntryPtr displayDriverPtr;
	
	CommandConsole() {
		
		promptCharacter = '>';
		promptState     = 0;
		cursorLine      = 0;
		cursorPos       = 0;
		
		keyboard_string_length = 0;
		clearString();
		
		keyboardState=0;
		lastChar=0;
		
	}
	
	void initiate(void) {
		
		const char deviceName[] = "con";
		displayDriverPtr = getFuncAddress(deviceName, sizeof(deviceName));
		
	}
	
	// Shift the display up one line
	void shiftUp(void) {callExtern(displayDriverPtr, 0x07); _delay_ms(100);}
	
	// Add a const char string to the console
	void print(char character) {
		//displayDriver.writeChar(character, cursorLine, cursorPos);
		callExtern(displayDriverPtr, 0x09, (uint8_t&)character, cursorLine, cursorPos);
		cursorPos++;
		return;
	}
	void print(int number) {
		
		char numberString[8];
		uint8_t place = intToString(number, numberString) + 1;
		
		for (uint8_t i=0; i<place; i++) {
			callExtern(displayDriverPtr, 0x09, (uint8_t&)numberString[i], cursorLine, cursorPos);
			cursorPos++;
		}
		return;
	}
	void print(const char charArray[], uint8_t string_length) {
		for (uint8_t i=0; i<string_length; i++) {
			callExtern(displayDriverPtr, 0x09, (uint8_t&)charArray[i], cursorLine, cursorPos);
			cursorPos++;
		}
		return;
	}
	
	void print(string& charString, uint8_t string_length) {
		for (uint8_t i=0; i<string_length; i++) {
			callExtern(displayDriverPtr, 0x09, (uint8_t&)charString[i], cursorLine, cursorPos);
			cursorPos++;
		}
		return;
	}
	
	// Add new blank line
	void printLn(void) {cursorPos = 0; if (cursorLine < 3) {cursorLine++;} else {shiftUp();} return;}
	
	// Display a command prompt
	void printPrompt(void) {
		cursorPos = 1;
		uint8_t promptPos = 0;
		callExtern(displayDriverPtr, 0x09, (uint8_t&)promptCharacter, cursorLine, promptPos);
		callExtern(displayDriverPtr, 0x00, cursorLine, cursorPos);
	}
	
	// Clear the keyboard string
	void clearString(void) {for (uint8_t i=0; i <= 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
	
};
CommandConsole console;


