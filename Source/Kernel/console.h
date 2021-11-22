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
	
	// Shift the display up one line
	void shiftUp(void) {displayDriver.frameShiftUp();_delay_ms(100);}
	
	// Add a const char string to the console
	void print(const char charArray[], uint8_t string_length) {
		displayDriver.writeString(charArray, string_length, cursorLine, cursorPos);
		cursorPos+=string_length;
		return;
	}
	void print(string& charString, uint8_t string_length) {
		displayDriver.writeString(charString, string_length, cursorLine, cursorPos);
		cursorPos+=string_length;
		return;
	}
	
	// Add new blank line
	void newLine(void) {cursorPos = 0; if (cursorLine < 3) {cursorLine++;} else {shiftUp();} return;}
	
	// Display a command prompt
	void newPrompt(void) {
		cursorPos = 1;
		displayDriver.writeChar(promptCharacter, cursorLine, 0);
		displayDriver.cursorSetPosition(cursorLine, 1);
	}
	
	// Clear the keyboard string
	void clearString(void) {for (uint8_t i=0; i <= 20; i++) {keyboard_string[i] = 0x20;} keyboard_string_length=0;}
	
};
CommandConsole console;


