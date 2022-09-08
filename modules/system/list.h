//
// List command

void command_list(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ModuleLoaderList {
	
	uint32_t target_device;
	uint32_t offset_device;
	uint16_t page_counter;
	
	ModuleLoaderList() {
		
		target_device = 0x40000;
		offset_device = 0x40000;
		page_counter  = 0;
		
	}
}static moduleLoaderList;


void command_list(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof("list")];
	uint8_t param1 = console.keyboard_string[sizeof("list") + 2];
	
	// Set target device address
	if (param0 == 't') {
		
		if ((param1 > '0') & (param1 < '6')) {
			moduleLoaderList.target_device = 0x30000 + (0x10000 * (param1 - '0'));
			moduleLoaderList.page_counter = 0;
			moduleLoaderList.offset_device = moduleLoaderList.target_device;
		}
		
		return;
	}
	
	// Set page counter
	if (param0 == 'p') {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("list") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("list") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		   (((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			moduleLoaderList.page_counter = string_get_hex_char(hex_string);
			moduleLoaderList.offset_device = moduleLoaderList.target_device;
			
			if (hex_string[0] == ' ') {
				hex_string[0] = hex_string[1];
				hex_string[1] = '0';
			}
			
		}
		
		char msg_str[] = "Page ";
		console.print(msg_str, sizeof(msg_str));
		console.printInt(moduleLoaderList.page_counter);
		console.printLn();
		
		return;
	}
	
	// Reverse the page counter
	if (param0 == 'r') {
		if (moduleLoaderList.page_counter > 1) 
			moduleLoaderList.page_counter--;
	} else {
		if (moduleLoaderList.page_counter < 0xffff) 
			moduleLoaderList.page_counter++;
	}
	
	moduleLoaderList.offset_device = moduleLoaderList.target_device + (moduleLoaderList.page_counter * 60);
	
	WrappedPointer pointer;
	char byte;
	
	// List data
	for (uint32_t i=(moduleLoaderList.offset_device - 60); i < moduleLoaderList.offset_device; i++) {
		
		pointer.address = i;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDevice, 0x11, (uint8_t&)byte);
		
		if (param1 == 'h') {
			console.printHex(byte);
		} else {
			console.printChar(byte);
		}
		
	}
	
	return;
}

