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
	
	uint8_t param0 = console.keyboard_string[sizeof(__MODULE_NAME_)];
	
	if (param0 == 'p') {fs.list_directory(1);} else {fs.list_directory(0);}
	
	return;
}


#undef __MODULE_NAME_








#endif

