#ifndef _DIR_COMMAND__
#define _DIR_COMMAND__

void command_dir(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof("dir")];
	uint8_t param1 = console.keyboard_string[sizeof("dir") + 1];
	
	uint8_t pageflag = 0;
	if ((param0 == '-') & (param1 == 'p')) 
		pageflag = 1;
	
	// List contents of the current device
	call_extern(storageDevice, 0x2f, pageflag);
	
	return;
}

#endif

