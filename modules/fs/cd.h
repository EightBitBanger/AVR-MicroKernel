#ifndef _CD_FUNCTION__
#define _CD_FUNCTION__

void command_cd(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "cd"

struct ModuleLoaderCd {
	
	ModuleLoaderCd() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_cd, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderCd;


void command_cd(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1 = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	uint8_t param2 = console.keyboard_string[sizeof(__MODULE_NAME_) + 3];
	
	if ((param0 >= 'a') & (param0 <= 'e')) 
		console.promptString[0] = (param0 - 0x20);
	
	return;
}


#undef __MODULE_NAME_

#endif

