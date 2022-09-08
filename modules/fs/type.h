#ifndef _TYPE_FUNCTION__
#define _TYPE_FUNCTION__

void command_type(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ModuleLoaderType {
	
	uint32_t page_offset;
	uint32_t byte_offset;
	
	ModuleLoaderType() {
		
		page_offset = 0;
		byte_offset = 0;
		
	}
}static moduleLoaderType;

void command_type(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	char filename[10];
	for (uint8_t i=0; i < 10; i++) 
		filename[i] = 0x20;
	
	// Get file name from keyboard string
	for (uint8_t a=0; a < 10; a++) 
		filename[a] = console.keyboard_string[sizeof("type") + a];
	
	// Set the sector page offset
	if ((filename[0] == 'p') & (filename[1] == ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("type") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("type") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			moduleLoaderType.page_offset = string_get_hex_char(hex_string);
			
			if (hex_string[0] == ' ') {
				hex_string[0] = hex_string[1];
				hex_string[1] = '0';
			}
			
		}
		
		char msg_str[] = "Page offset ";
		console.print(msg_str, sizeof(msg_str));
		console.printInt(moduleLoaderType.page_offset);
		console.printLn();
		
		return;
	}
	
	// Set the byte offset
	if ((filename[0] == 'b') & (filename[1] == ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("type") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("type") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			moduleLoaderType.byte_offset = string_get_hex_char(hex_string);
			
			if (hex_string[0] == ' ') {
				hex_string[0] = hex_string[1];
				hex_string[1] = '0';
			}
			
		}
		
		char msg_str[] = "Byte offset ";
		console.print(msg_str, sizeof(msg_str));
		console.printInt(moduleLoaderType.byte_offset);
		console.printLn();
		
		return;
	}
	
	// Write a char byte to the set address
	if ((filename[0] == 'w') & (filename[1] == ' ') & (filename[2] != ' ')) {
		
		char str_val[32];
		
		for (uint8_t a=0; a < 32; a++) {
			
			str_val[a] = console.keyboard_string[sizeof("type") + 2 + a];
			
			WrappedPointer pointer;
			pointer.address = ((moduleLoaderType.page_offset + 1) * 32) + moduleLoaderType.byte_offset + 1;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			
			if (str_val[a] == 0x20) break;
			
			call_extern(storageDevice, 0x10, (uint8_t&)str_val[a]);
			
			if (moduleLoaderType.byte_offset >= (32 - 2)) {
				moduleLoaderType.byte_offset = 0;
			} else {
				moduleLoaderType.byte_offset++;
			}
			
		}
		
		
		return;
	}
	
	// Write a HEX byte to the set address
	if ((filename[0] == 'h') & (filename[1] == ' ') & (filename[2] != ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("type") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("type") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			uint8_t byte = string_get_hex_char(hex_string);
			
			WrappedPointer pointer;
			pointer.address = ((moduleLoaderType.page_offset + 1) * 32) + moduleLoaderType.byte_offset + 1;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			
			call_extern(storageDevice, 0x10, byte);
			
			moduleLoaderType.byte_offset++;
			
			console.printChar(byte);
			console.printLn();
			
			return;
		}
		
	}
	
	
	
	// Open file
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, (uint8_t&)filename[a]);
	
	uint8_t return_value;
	call_extern(storageDevice, 0x0d, return_value);
	
	// Check file exits
	if (return_value != 0) {
		
		// Read out the file
		WrappedPointer pointer;
		char byte;
		for (uint32_t i=((moduleLoaderType.page_offset + 1) * 32); i < ((moduleLoaderType.page_offset + 2) * 32) + 1; i++) {
			
			pointer.address = i;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			
			call_extern(storageDevice, 0x0f, (uint8_t&)byte);
			
			console.printChar(byte);
			
		}
		
		console.printLn();
	}
	
	uint8_t blank_byte = 0x20;
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, blank_byte);
	
	return;
}

#endif

