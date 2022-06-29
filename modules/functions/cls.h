//
// Clear screen command

void command_clear_screen(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "cls"

struct ModuleLoaderCls {
	ModuleLoaderCls() {
		load_library(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)command_clear_screen, _DEVICE_TYPE_MODULE__);
	}
}static loadModuleCls;
#undef __MODULE_NAME_

void command_clear_screen(uint8_t function, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
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


