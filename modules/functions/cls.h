//
// Clear screen command

void command_clear_screen(void);

struct ModuleLoaderCls {
	ModuleLoaderCls() {
		load_module("cls",  4, command_clear_screen);
	}
}static loadModuleCls;

void command_clear_screen(void) {
	
	// Clear the buffer and mask
	console.clearBuffer();
	console.clearMask();
	console.clearString();
	
	// Reset the cursor
	console.cursorLine  = 0;
	console.cursorPos   = 1;
	
	_delay_ms(120);
	
	// Initiate the prompt
	console.promptState = 1;
	
	return;
}


