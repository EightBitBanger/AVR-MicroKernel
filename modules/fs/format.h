//
// Storage format function

#define _FORMAT_MULTIPLIER__   1024

void command_format(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ModuleLoaderFormat {
	
	uint8_t format_size;
	
	ModuleLoaderFormat() {
		
		format_size = 8;
		
	}
	
}static moduleLoaderFormat;


void command_format(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof("format")];
	uint8_t param1 = console.keyboard_string[sizeof("format") + 2];
	
	// Set the format size
	if (param0 == 's') {
		
		if ((param1 > '0') & (param1 <= '9')) moduleLoaderFormat.format_size = param1 - '0';
		if (param1 == 'a') moduleLoaderFormat.format_size = 16;
		if (param1 == 'b') moduleLoaderFormat.format_size = 32;
		
		char in_prog_str[] = "Capacity ";
		console.print(in_prog_str, sizeof(in_prog_str));
		console.printInt(moduleLoaderFormat.format_size * _FORMAT_MULTIPLIER__);
		console.printLn();
		
		return;
	}
	
	if ((param0 < 'a') | (param0 > 'f')) 
		return;
	
	// Format the storage device
	WrappedPointer pointer;
	uint8_t byte = 0x20;
	uint8_t flag = 0x00;
	uint8_t write_counter = 0;
	
	uint32_t base_address = 0x30000 + (0x10000 * (param0 - 'a' + 1));
	
	char in_prog_str[] = "Formatting device...";
	console.print(in_prog_str, sizeof(in_prog_str));
	console.printLn();
	
	for (uint32_t i=base_address; i < base_address + (moduleLoaderFormat.format_size * _FORMAT_MULTIPLIER__); i++) {
		
		pointer.address = i;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		
		if (write_counter == 0) {
			call_extern(storageDevice, 0x12, (uint8_t&)flag); // Sector state flag
		} else {
			call_extern(storageDevice, 0x12, (uint8_t&)byte); // Sector data byte
		}
		
		// 32 byte page counter
		if (write_counter >= 31) {write_counter=0; _delay_ms(5);} else {write_counter++;}
		
	}
	
	// Initiate first sector
	char header[] = {0x13,'s','t','o','r','a','g','e',' ',' ',  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00};
	write_counter=0;
	
	for (uint8_t i=0; i < sizeof(header); i++) {
		pointer.address = i + base_address;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDevice, 0x12, (uint8_t&)header[i]);
		
		// 32 byte page counter
		if (write_counter >= 31) {write_counter=0; _delay_ms(5);} else {write_counter++;}
		
	}
	
	// Complete message
	char complete_str[] = "Format complete.";
	console.print(complete_str, sizeof(complete_str));
	console.printLn();
	
	return;
}


