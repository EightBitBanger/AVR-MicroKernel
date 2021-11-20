//
// Clear screen command module

void command_clear_screen(void);

struct ModuleClearScreen {
	
	ModuleClearScreen() {
		
		// Command name
		const char name[] = "cls";
		
		// Install the function pointer
		kernel.installFunction(&command_clear_screen, name, sizeof(name));
		
		return;
	}
	
};
static ModuleClearScreen moduleClearScreen;

void command_clear_screen(void) {
	
	// Clear the buffer and mask
	displayDriver.clearBuffer();
	displayDriver.clearMask();
	kernel.clearKeyboardString();
	
	// Reset the cursor
	kernel.cursorLine  = 0;
	kernel.cursorPos   = 1;
	
	_delay_ms(100);
	
	// Initiate the prompt
	kernel.consoleNewPrompt();
	
	kernel.promptState = 1;
	
	return;
}


