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
	
	if (param0 == 's') {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
		uint8_t char_a = console.keyboard_string[sizeof(__MODULE_NAME_) + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			moduleLoaderMk.file_size = string_get_hex_char(hex_string);
			
			console.printHex(moduleLoaderMk.file_size);
			console.printLn();
			
			if (hex_string[0] == ' ') {
				hex_string[0] = hex_string[1];
				hex_string[1] = '0';
			}
			
		}
		return;
	}
	
	
	if ((param0 >= 'a') & (param0 <= 'e')) 
		console.promptString[0] = (param0 - 0x20);
	
	return;
}


#undef __MODULE_NAME_

#endif

