#ifndef _CD_FUNCTION__
#define _CD_FUNCTION__

void command_cd(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof("cd")];
	
	if ((param0 >= 'a') & (param0 <= 'e')) 
		console.promptString[0] = (param0 - 0x20);
	
	return;
}

#endif

