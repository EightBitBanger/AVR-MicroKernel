//
// List command

void command_list(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ModuleLoaderList {
	
	uint16_t page_counter;
	
	ModuleLoaderList() {
		
		page_counter = 0;
		
	}
}static moduleLoaderList;


void command_list(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	volatile uint32_t offset_device;
	volatile uint32_t target_device;
	
	uint8_t param0 = console.keyboard_string[sizeof("list")];
	uint8_t param1 = console.keyboard_string[sizeof("list") + 2];
	
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
	
	// Set target device address
	if ((console.promptString[0] >= 'A') & (console.promptString[0] < 'F'))
		target_device = 0x30000 + (0x10000 * (console.promptString[0] - 0x40));
	
	if (console.promptString[0] == '/')
		target_device = _VIRTUAL_STORAGE_ADDRESS__;
	
	char byte;
	
	// List data
	for (uint8_t i=0; i < 60; i++) {
		
		offset_device = target_device + ((moduleLoaderList.page_counter * 60) + (i - 60));
		
		fs.read(offset_device, byte);
		
		console.printChar(byte);
		
	}
	
	return;
}