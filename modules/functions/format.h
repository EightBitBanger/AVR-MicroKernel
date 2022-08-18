//
// Storage format function

void command_format(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "format"
#define _FORMAT_MULTIPLIER__   1024

struct ModuleLoaderFormat {
	
	uint8_t format_size;
	
	ModuleLoaderFormat() {
		
		format_size = 1;
		
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_format, DEVICE_TYPE_MODULE);
	}
	
}static moduleLoaderFormat;


void command_format(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Bus bus;
	bus.waitstate_read  = 4;
	bus.waitstate_write = 1000;
	
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1 = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	
	//if ((param0 < '1') | (param0 > '9')) return;
	//if ((param0 < 'a') | (param0 > 'z')) return;
	
	// Set the size for formatting
	if (param0 == 's') {
		
		if ((param1 < '1') | (param1 > '9')) return;
		
		moduleLoaderFormat.format_size = param1 - '0';
		
		char in_prog_str[] = "Capacity ";
		console.print(in_prog_str, sizeof(in_prog_str));
		console.printInt(moduleLoaderFormat.format_size * _FORMAT_MULTIPLIER__);
		console.printLn();
		
	}
	
	// Format the storage device
	if (param0 == 'f') {
		
		WrappedPointer pointer;
		uint8_t byte = ' ';
		
		char in_prog_str[] = "Formatting device...";
		console.print(in_prog_str, sizeof(in_prog_str));
		console.printLn();
		
		for (uint32_t i=0x40000; i < 0x40000 + (moduleLoaderFormat.format_size * _FORMAT_MULTIPLIER__); i++) {
			pointer.address = i;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			call_extern(storageDevice, 0x01, (uint8_t&)byte);
		}
		
		// Initiate device ID
		char header[] = {0x13,'s','t','o','r','a','g','e',' ',' '};
		
		for (uint8_t i=0; i < sizeof(header); i++) {
			pointer.address = i + 0x40000;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			call_extern(storageDevice, 0x01, (uint8_t&)header[i]);
		}
		
		// Complete message
		char complete_str[] = "Format complete.";
		console.print(complete_str, sizeof(complete_str));
		console.printLn();
		
		return;
	}
	
	
	/*
	// List data
	WrappedPointer pointer;
	char byte;
	uint32_t device_address = 0x30000 + (0x10000 * (param0 - '0'));
	for (uint16_t i=0; i <= 40; i++) {
		
		pointer.address = (device_address + i);
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDevice, 0x00, (uint8_t&)byte);
		
		console.printChar(byte);
		
	}
	*/
	
	return;
}


#undef __MODULE_NAME_

