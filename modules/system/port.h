//
// Port control function

void __port_control_(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "port"

struct ModuleLoaderPort {
	
	uint32_t slot_address;
	uint32_t port_address;
	
	ModuleLoaderPort() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (void(*)())__port_control_, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderPort;


void __port_control_(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Bus device_bus;
	
	device_bus.waitstate_read  = 10;
	device_bus.waitstate_write = 10;
	
	uint8_t param0  = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1  = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	
	// GPIO Port selection
	if (param0 == 'p') {
		
		if ((param1 >= 'a') & (param1 <= 'd')) 
			moduleLoaderPort.port_address = param1 - 'a';
		
		char msg_string[] = "Port ";
		console.print(msg_string, sizeof(msg_string));
		console.printChar(moduleLoaderPort.port_address + 'a');
		console.printLn();
		return;
	}
	
	// Hardware slot selection
	if (param0 == 's') {
		
		if ((param1 >= '1') & (param1 <= '5')) 
			moduleLoaderPort.slot_address = ((param1 - '0') * 0x10000) + 0x30000;
		
		char msg_string[] = "Slot ";
		console.print(msg_string, sizeof(msg_string));
		console.printChar( ((moduleLoaderPort.slot_address - 0x30000) / 0x10000) + '0');
		console.printLn();
		return;
	}
	
	// Bit field value
	if (param0 == 'b') {
		uint8_t byte = 0x00;
		
		if (console.keyboard_string[7]  == 0x31) byte |= 0b00000001;
		if (console.keyboard_string[8]  == 0x31) byte |= 0b00000010;
		if (console.keyboard_string[9]  == 0x31) byte |= 0b00000100;
		if (console.keyboard_string[10] == 0x31) byte |= 0b00001000;
		if (console.keyboard_string[11] == 0x31) byte |= 0b00010000;
		if (console.keyboard_string[12] == 0x31) byte |= 0b00100000;
		if (console.keyboard_string[13] == 0x31) byte |= 0b01000000;
		if (console.keyboard_string[14] == 0x31) byte |= 0b10000000;
		
		uint32_t address = (moduleLoaderPort.slot_address + moduleLoaderPort.port_address);
		
		bus_write_byte(device_bus, address, byte);
		
		return;
	}
	
	// HEX value
	if ((console.keyboard_string[5] == '0') && (console.keyboard_string[6] == 'x')) {
		uint8_t value=0;
		
		char string[2];
		string[1] = console.keyboard_string[7];
		string[0] = console.keyboard_string[8];
		
		value = string_get_hex_char(string);
		
		uint32_t address = (moduleLoaderPort.slot_address + moduleLoaderPort.port_address);
		
		bus_write_byte(device_bus, address, value);
		
		return;
	}
	
	//
	// No parameters - help anyone ?
	if (param0 == 0x20) {
		
		char help_line_a[] = "port p n - set port";
		char help_line_b[] = "port s n - set slot";
		char help_line_c[] = "port 0x00 - set data";
		
		console.print(help_line_a, sizeof(help_line_a)); console.printLn();
		console.print(help_line_b, sizeof(help_line_b)); console.printLn();
		console.print(help_line_c, sizeof(help_line_c)); console.printLn();
		
		return;
	}
	
	return;
}

#undef __MODULE_NAME_

