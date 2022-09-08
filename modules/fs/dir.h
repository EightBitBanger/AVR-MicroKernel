#ifndef _DIR_COMMAND__
#define _DIR_COMMAND__

void command_dir(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t state = 0;
	if (console.keyboard_string[sizeof("dir")] == 'p') {state = 1;}
	
	call_extern(storageDevice, 0x2f, state);
	
	return;
}

#endif

