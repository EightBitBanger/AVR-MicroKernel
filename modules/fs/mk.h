#ifndef _MK_FUNCTION__
#define _MK_FUNCTION__

char msg_file_created[] = "File created.";
char msg_file_exists[]  = "File already exists.";
char msg_file_not_found[]  = "File not found.";

void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct __ModuleLoaderMk__ {
	
	uint32_t file_size;
	
	__ModuleLoaderMk__() {
		
		file_size = SECTOR_SIZE;
		
	}
}static __moduleLoaderMk__;


void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	// Clear the storage filename reference
	for (uint8_t name_length=0; name_length < 10; name_length++) {
		uint8_t str_char = 0x20;
		call_extern(storageDevice, name_length, (uint8_t&)str_char);
	}
	
	// Get filename parameter
	char filename[11] = "          ";
	uint8_t name_length=0;
	for (name_length=0; name_length < 10; name_length++) {
		uint8_t str_char = console.keyboard_string[sizeof("mk") + name_length];
		if (str_char == ' ') break;
		call_extern(storageDevice, name_length, (uint8_t&)str_char);
		filename[name_length] = str_char;
	}
	
	if ((filename[0] == 's') & (filename[1] == ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("mk") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("mk") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			__moduleLoaderMk__.file_size = string_get_hex_char(hex_string);
			
			char file_size[] = "File size (hex) ";
			console.print(file_size, sizeof(file_size));
			console.printHex(__moduleLoaderMk__.file_size);
			console.printLn();
			
			if (hex_string[0] == ' ') {
				hex_string[0] = hex_string[1];
				hex_string[1] = '0';
			}
			
			return;
		}
	}
	
	// Name length no less than three
	if (name_length < 2) {
		return;
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
	pointer.address = __moduleLoaderMk__.file_size;
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

#endif

