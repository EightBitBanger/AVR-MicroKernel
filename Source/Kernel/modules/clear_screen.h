//
// Clear screen command module

void command_clear_screen(void);

struct ModuleClearScreen {
	
	ModuleClearScreen() {
		
		const char command_name[] = "cls";
		kernel.function.install(&command_clear_screen, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleClearScreen moduleClearScreen;

void command_clear_screen(void) {
	
	// Clear the buffer and mask
	displayDriver.clearBuffer();
	displayDriver.clearMask();
	kernel.console.consoleClearString();
	
	// Reset the cursor
	kernel.console.cursorLine  = 0;
	kernel.console.cursorPos   = 1;
	
	_delay_ms(200);
	
	// Initiate the prompt
	kernel.console.consoleNewPrompt();
	
	kernel.console.promptState = 1;
	
	return;
}


