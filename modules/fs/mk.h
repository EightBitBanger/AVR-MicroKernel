#ifndef _MK_FUNCTION__
#define _MK_FUNCTION__

void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct __ModuleLoaderMk__ {
	
	uint32_t file_size;
	
	__ModuleLoaderMk__() {
		
		file_size = 1;
		
	}
}static __moduleLoaderMk__;


void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	// Check the volume header of the current device
	uint32_t current_device = set_device_scope();
	
	if (fs.device_check_header(current_device - SECTOR_SIZE) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	WrappedPointer pointer;
	
	// Clear the storage filename reference
	for (uint8_t name_length=0; name_length < 10; name_length++) {
		uint8_t str_char = 0x20;
		call_extern(storageDevice, name_length, (uint8_t&)str_char);
	}
	
	// Get filename parameter
	char filename[10];
	for (uint8_t a=0; a < 10; a++) 
		filename[a] = 0x20;
	
	uint8_t name_length=0;
	for (name_length=0; name_length < 10; name_length++) {
		uint8_t str_char = console.keyboard_string[sizeof("mk") + name_length];
		if (str_char == ' ') break;
		call_extern(storageDevice, name_length, (uint8_t&)str_char);
		filename[name_length] = str_char;
	}
	
	if ((filename[0] == '-') & (filename[1] == 's')) {
		
		char msg_file_size[] = "Size 0x";
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("mk") + 3];
		uint8_t char_a = console.keyboard_string[sizeof("mk") + 4];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			if ((char_a != 0) | (char_b != 0)) 
				__moduleLoaderMk__.file_size = string_get_hex_char(hex_string);
			
		}
		
		pointer.address = __moduleLoaderMk__.file_size * SECTOR_SIZE;
		
		console.print(msg_file_size, sizeof(msg_file_size));
		console.printHex(pointer.byte[3]);
		console.printHex(pointer.byte[2]);
		console.printHex(pointer.byte[1]);
		console.printHex(pointer.byte[0]);
		console.printLn();
		
		return;
	}
	
	// Check if the file already exists
	WrappedPointer return_value;
	call_extern(storageDevice, 0x0c, return_value.byte_t[0], return_value.byte_t[1], return_value.byte_t[2], return_value.byte_t[3]);
	
	if (return_value.address != 0) {
		console.print(msg_file_already_exists, sizeof(msg_file_already_exists));
		console.printLn();
		return;
	}
	
	// Set the file size
	uint32_t sector_address = __moduleLoaderMk__.file_size * SECTOR_SIZE;
	
	uint8_t attributes[] = " rw ";
	
	file_create(filename, __moduleLoaderMk__.file_size, attributes);
	
	return;
}

#endif

