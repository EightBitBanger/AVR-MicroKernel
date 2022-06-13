//
// Clear screen command

void command_clear_screen(void);

#define __MODULE_NAME_  "cls"

struct ModuleLoaderCls {
	ModuleLoaderCls() {
		load_module(__MODULE_NAME_,  sizeof(__MODULE_NAME_), command_clear_screen);
	}
}static loadModuleCls;
#undef __MODULE_NAME_

void command_clear_screen(void) {
	
	// Clear the buffer and mask
	console.clearBuffer();
	console.clearMask();
	console.clearKeyboardString();
	
	// Reset the cursor
	console.cursorLine  = 0;
	console.cursorPos   = 1;
	
	_delay_ms(120);
	
	// Initiate the prompt
	console.promptState = 1;
	
	return;
}


