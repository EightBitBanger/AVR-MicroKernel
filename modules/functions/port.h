//
// Port control function

void __io_control_(void);

struct ModuleLoaderIOport {
	ModuleLoaderIOport() {
		load_module("port",  5, __io_control_);
	}
}static moduleLoaderIOport;

void executable_task(void);

//
// Entry point






void __io_control_(void) {
	
	Bus device_bus;
	device_bus.waitstate_read  = 0;
	device_bus.waitstate_write = 0;
	
	// Port selection
	if ((console.keyboard_string[5] == 's') && (console.keyboard_string[6] == 'e') && (console.keyboard_string[7] == 't')) {
		
		if ((console.keyboard_string[9] == '1') == 1) {
			
		}
		
		return;
	}
	
	// HEX value
	if ((console.keyboard_string[5] == '0') && (console.keyboard_string[6] == 'x')) {
		uint8_t value=0;
		
		char string[2];
		string[1] = console.keyboard_string[7];
		string[0] = console.keyboard_string[8];
		
		value = string_get_hex(string);
		
		bus_write_byte(device_bus, 0xd0000, value);
		
		return;
	}
	
	
	// Bit field value
	uint8_t byte = 0x00;
	
	if (console.keyboard_string[5]  == 0x31) byte |= 0b00000001;
	if (console.keyboard_string[6]  == 0x31) byte |= 0b00000010;
	if (console.keyboard_string[7]  == 0x31) byte |= 0b00000100;
	if (console.keyboard_string[8]  == 0x31) byte |= 0b00001000;
	if (console.keyboard_string[9]  == 0x31) byte |= 0b00010000;
	if (console.keyboard_string[10] == 0x31) byte |= 0b00100000;
	if (console.keyboard_string[11] == 0x31) byte |= 0b01000000;
	if (console.keyboard_string[12] == 0x31) byte |= 0b10000000;
	
	bus_write_byte(device_bus, 0xd0000, byte);
	
	return;
}


