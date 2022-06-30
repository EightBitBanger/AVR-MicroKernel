//
// Port control function

void __io_control_(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "port"

struct ModuleLoaderPort {
	
	uint32_t address;
	
	ModuleLoaderPort() {
		address=0x00000;
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)__io_control_, _DEVICE_TYPE_MODULE__);
	}
}static moduleLoaderPort;
#undef __MODULE_NAME_


void __io_control_(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Bus device_bus;
	
	device_bus.waitstate_read  = 10;
	device_bus.waitstate_write = 10;
	
	// Port selection
	if ((console.keyboard_string[5] == 'a') == 1) {
		moduleLoaderPort.address = 0x60000;
		return;
	}
	if ((console.keyboard_string[5] == 'b') == 1) {
		moduleLoaderPort.address = 0x60001;
		return;
	}
	if ((console.keyboard_string[5] == 'c') == 1) {
		moduleLoaderPort.address = 0x60002;
		return;
	}
	if ((console.keyboard_string[5] == 'd') == 1) {
		moduleLoaderPort.address = 0x60003;
		return;
	}
	
	// HEX value
	if ((console.keyboard_string[5] == '0') && (console.keyboard_string[6] == 'x')) {
		uint8_t value=0;
		
		char string[2];
		string[1] = console.keyboard_string[7];
		string[0] = console.keyboard_string[8];
		
		value = string_get_hex_char(string);
		
		bus_write_byte(device_bus, moduleLoaderPort.address, value);
		
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
	
	bus_write_byte(device_bus, moduleLoaderPort.address, byte);
	
	return;
}


