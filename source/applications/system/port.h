//
// GPIO port control function

void command_port(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define PORT_ADDRESS  0x90002

struct ____moduleLoaderPort____ {
	
	uint32_t slot_address;
	uint32_t port_address;
	
	____moduleLoaderPort____() {
		
		slot_address = PORT_ADDRESS;
		port_address = 0;
		
	}
}static __moduleLoaderPort__;


void __port_control_(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Bus device_bus;
	
	device_bus.waitstate_read  = 10;
	device_bus.waitstate_write = 10;
	
	string_lower(console.keyboard_string, sizeof("port") + 3);
	uint8_t param0  = console.keyboard_string[sizeof("port")];
	uint8_t param1  = console.keyboard_string[sizeof("port") + 1];
	uint8_t param2  = console.keyboard_string[sizeof("port") + 3];
	
	// GPIO Port selection
	if ((param0 == '-') & (param1 == 'p')) {
		
		if ((param2 >= 'a') & (param2 <= 'd')) 
			__moduleLoaderPort__.port_address = param2 - 'a';
		
		if ((param2 >= '0') & (param2 <= '5'))
		__moduleLoaderPort__.port_address = param2 - '0';
		
		char msg_string[] = "Port ";
		console.print(msg_string, sizeof(msg_string));
		console.printChar(__moduleLoaderPort__.port_address + 'a');
		console.printLn();
		return;
	}
	
	// Bit field value
	if ((param0 == '-') & (param1 == 'b')) {
		uint8_t byte = 0x00;
		
		if (console.keyboard_string[sizeof("port") + 3]  == 0x31) byte |= 0b00000001;
		if (console.keyboard_string[sizeof("port") + 4]  == 0x31) byte |= 0b00000010;
		if (console.keyboard_string[sizeof("port") + 5]  == 0x31) byte |= 0b00000100;
		if (console.keyboard_string[sizeof("port") + 6]  == 0x31) byte |= 0b00001000;
		if (console.keyboard_string[sizeof("port") + 7]  == 0x31) byte |= 0b00010000;
		if (console.keyboard_string[sizeof("port") + 8]  == 0x31) byte |= 0b00100000;
		if (console.keyboard_string[sizeof("port") + 9]  == 0x31) byte |= 0b01000000;
		if (console.keyboard_string[sizeof("port") + 10] == 0x31) byte |= 0b10000000;
		
		uint32_t address = (__moduleLoaderPort__.slot_address + __moduleLoaderPort__.port_address);
		
		bus_write_byte(device_bus, address, byte);
		
		return;
	}
	
	// HEX value
	if ((console.keyboard_string[sizeof("port")] == 0x00) | (console.keyboard_string[sizeof("port")] == 0x20)) console.keyboard_string[sizeof("port")] = '0';
	if (((console.keyboard_string[sizeof("port")] >= '0') & (console.keyboard_string[sizeof("port")] <= '9')) | 
	    ((console.keyboard_string[sizeof("port")] >= 'a') & (console.keyboard_string[sizeof("port")] <= 'f'))) {
		
		if ((console.keyboard_string[sizeof("port") + 1] == 0x00) | (console.keyboard_string[sizeof("port") + 1] == 0x20)) console.keyboard_string[sizeof("port") + 1] = '0';
		if (((console.keyboard_string[sizeof("port") + 1] >= '0') & (console.keyboard_string[sizeof("port") + 1] <= '9')) |
			((console.keyboard_string[sizeof("port") + 1] >= 'a') & (console.keyboard_string[sizeof("port") + 1] <= 'f'))) {
			
			uint8_t value=0;
			
			char string[2];
			string[1] = console.keyboard_string[sizeof("port")];
			string[0] = console.keyboard_string[sizeof("port") + 1];
			
			value = string_get_hex_char(string);
			
			uint32_t address = (__moduleLoaderPort__.slot_address + __moduleLoaderPort__.port_address);
			
			bus_write_byte(device_bus, address, value);
			
		}
		return;
	}
	
	//
	// No parameters - help anyone ?
	if (param0 == 0x20) {
		
		char help_line_a[] = "port s n - set slot";
		char help_line_b[] = "port p n - set port";
		char help_line_c[] = "port 0x00 - write GPIO";
		
		console.print(help_line_a, sizeof(help_line_a)); console.printLn();
		console.print(help_line_b, sizeof(help_line_b)); console.printLn();
		console.print(help_line_c, sizeof(help_line_c)); console.printLn();
		
		return;
	}
	
	return;
}

