// Peek a memory address

void command_peek(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "peek"

struct ModuleLoaderPeek {
	ModuleLoaderPeek() {
		load_library(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)command_peek, _DEVICE_TYPE_MODULE__);
	}
} static moduleLoaderPeek;
#undef __MODULE_NAME_


void command_peek(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Bus device_bus;
	
	device_bus.waitstate_read  = 4;
	device_bus.waitstate_write = 4;
	
	// HEX value
	if ((console.keyboard_string[5] == '0') && (console.keyboard_string[6] == 'x')) {
		
		char string_a[2];
		char string_b[2];
		string_b[1] = console.keyboard_string[7];
		string_b[0] = console.keyboard_string[8];
		string_a[1] = console.keyboard_string[9];
		string_a[0] = console.keyboard_string[10];
		
		uint16_t value_a = string_get_hex_char(string_a);
		uint16_t value_b = string_get_hex_char(string_b);
		
		console.printInt( value_a );
		console.printLn();
		console.printInt( value_b );
		console.printLn();
		
		WrappedPointer pointer;
		pointer.word[0] = value_a;
		pointer.word[1] = value_b;
		
		console.printInt( pointer.address );
		console.printLn();
		
		//char return_value;
		//bus_read_byte(device_bus, value, return_value);
		
		return;
	}
	
	return;
}


