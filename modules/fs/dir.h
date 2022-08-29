#ifndef _DIR_COMMAND__
#define _DIR_COMMAND__

void command_dir(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "dir"

struct ModuleLoaderDir {
	
	ModuleLoaderDir() {
		
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_dir, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderDir;


void command_dir(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t state = 0;
	if (console.keyboard_string[sizeof(__MODULE_NAME_)] == 'p') {state = 1;}
	
	call_extern(storageDevice, 0x2f, state);
	
	return;
}


#undef __MODULE_NAME_

#endif

