#ifndef _TYPE_FUNCTION__
#define _TYPE_FUNCTION__

void command_type(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "type"

struct ModuleLoaderType {
	
	uint32_t page_offset;
	uint32_t byte_offset;
	
	ModuleLoaderType() {
		
		page_offset = 0;
		byte_offset = 0;
		
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_type, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderType;

void command_type(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	char filename[10];
	for (uint8_t i=0; i < 10; i++) 
		filename[i] = 0x20;
	
	// Get file name from keyboard string
	for (uint8_t a=0; a < 10; a++) 
		filename[a] = console.keyboard_string[sizeof(__MODULE_NAME_) + a];
	
	// Set the sector page offset
	if ((filename[0] == 'p') & (filename[1] == ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
		uint8_t char_a = console.keyboard_string[sizeof(__MODULE_NAME_) + 3];
		
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
		uint8_t char_b = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
		uint8_t char_a = console.keyboard_string[sizeof(__MODULE_NAME_) + 3];
		
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
		
		char msg_str[] = "Byte offset";
		console.print(msg_str, sizeof(msg_str));
		console.printInt(moduleLoaderType.byte_offset);
		console.printLn();
		
		return;
	}
	
	// Write a byte to the selected 
	if ((filename[0] == 'w') & (filename[1] == ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
		uint8_t char_a = console.keyboard_string[sizeof(__MODULE_NAME_) + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			uint8_t byte = string_get_hex_char(hex_string);
			
			fs.file_write_byte((moduleLoaderType.page_offset * 32) + moduleLoaderType.byte_offset + 1, byte);
			
			moduleLoaderType.byte_offset++;
			
			console.printChar(byte);
			console.printLn();
			
		}
		
	}
	
	// Check file exits
	uint32_t file_size = fs.file_get_size(filename);
	if (file_size != 0) {
		
		if (fs.file_open(filename) == 1) {
			
			// Read out the file
			char byte;
			for (uint32_t i=((moduleLoaderType.page_offset + 1) * 32); i < ((moduleLoaderType.page_offset + 2) * 32); i++) {
				
				fs.file_read_byte(i, byte);
				console.printChar(byte);
				
			}
			console.printLn();
			moduleLoaderType.page_offset++;
			
		}
		
	}
	
	return;
}


#undef __MODULE_NAME_






#endif

