#ifndef _MK_FUNCTION__
#define _MK_FUNCTION__

void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "mk"

struct ModuleLoaderMk {
	
	uint32_t file_size;
	
	ModuleLoaderMk() {
		
		file_size = 1;
		
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_mk, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderMk;


void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Get filename parameter
	char filename[11] = "          ";
	for (uint8_t a=0; a < 10; a++) {
		uint8_t str_char = console.keyboard_string[sizeof(__MODULE_NAME_) + a];
		if (str_char == ' ') break;
		filename[a] = str_char;
	}
	
	fs.file_create(filename, moduleLoaderMk.file_size * 32);
	
	return;
}


#undef __MODULE_NAME_


#endif

