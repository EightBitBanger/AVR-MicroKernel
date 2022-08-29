#ifndef _MK_FUNCTION__
#define _MK_FUNCTION__

void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "mk"

char msg_file_created[] = "File created.";
char msg_file_exists[]  = "File already exists.";
char msg_file_not_found[]  = "File not found.";

struct ModuleLoaderMk {
	
	uint32_t file_size;
	
	ModuleLoaderMk() {
		
		file_size = 1;
		
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_mk, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderMk;


void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	// Get filename parameter
	char filename[11] = "          ";
	for (uint8_t a=0; a < 10; a++) {
		uint8_t str_char = console.keyboard_string[sizeof(__MODULE_NAME_) + a];
		if (str_char == ' ') break;
		call_extern(storageDevice, a, (uint8_t&)str_char);
		filename[a] = str_char;
	}
	
	if ((filename[0] == 's') & (filename[1] == ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
		uint8_t char_a = console.keyboard_string[sizeof(__MODULE_NAME_) + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			moduleLoaderMk.file_size = string_get_hex_char(hex_string);
			
			char file_size[] = "File size (hex) ";
			console.print(file_size, sizeof(file_size));
			console.printHex(moduleLoaderMk.file_size);
			console.printLn();
			
			if (hex_string[0] == ' ') {
				hex_string[0] = hex_string[1];
				hex_string[1] = '0';
			}
			
			return;
		}
	}
	
	// Check if the file already exists
	WrappedPointer return_value;
	call_extern(storageDevice, 0x0c, return_value.byte_t[0], return_value.byte_t[1], return_value.byte_t[2], return_value.byte_t[3]);
	
	if (return_value.address != 0) {
		console.print(msg_file_exists, sizeof(msg_file_exists));
		console.printLn();
		return;
	}
	
	// Set the file size
	WrappedPointer pointer;
	pointer.address = moduleLoaderMk.file_size * 32;
	call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	// Create the file
	uint8_t return_byte;
	call_extern(storageDevice, 0x0a, return_byte);
	
	uint8_t char_space=0x20;
	for (uint8_t a=0; a < 10; a++) 
		call_extern(storageDevice, a, char_space);
	
	if (return_byte != 0) {
		
		console.print(msg_file_created, sizeof(msg_file_created));
		console.printLn();
		
	}
	
	return;
}


#undef __MODULE_NAME_


#endif

