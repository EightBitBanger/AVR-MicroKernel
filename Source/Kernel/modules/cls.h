//
// Clear screen command

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
	console.clearString();
	
	// Reset the cursor
	console.cursorLine  = 0;
	console.cursorPos   = 1;
	
	_delay_ms(200);
	
	// Initiate the prompt
	console.newPrompt();
	
	console.promptState = 1;
	
	return;
}


