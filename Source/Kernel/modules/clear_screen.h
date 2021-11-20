//
// Clear screen command module

void command_clear_screen(void);

struct ModuleClearScreen {
	
	ModuleClearScreen() {
		
		const char command_name[] = "cls";
		kernel.installFunction(&command_clear_screen, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleClearScreen moduleClearScreen;

void command_clear_screen(void) {
	
	// Clear the buffer and mask
	displayDriver.clearBuffer();
	displayDriver.clearMask();
	kernel.consoleClearString();
	
	// Reset the cursor
	kernel.cursorLine  = 0;
	kernel.cursorPos   = 1;
	
	_delay_ms(100);
	
	// Initiate the prompt
	kernel.consoleNewPrompt();
	
	kernel.promptState = 1;
	
	return;
}


