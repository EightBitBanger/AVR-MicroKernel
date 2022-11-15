//
// Storage format function

char in_prog_str[] = "Formatting device...";

void command_fdisk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ModuleLoaderFormat {
	
	uint8_t format_size;
	
	ModuleLoaderFormat() {
		
		format_size = 8;
		
	}
	
}static moduleLoaderFormat;


void command_fdisk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	// Check the volume header of the current device
	uint32_t current_device = set_device_scope();
	
	uint8_t param0 = console.keyboard_string[sizeof("fdisk")];
	uint8_t param1 = console.keyboard_string[sizeof("fdisk") + 1];
	
	uint8_t param2 = console.keyboard_string[sizeof("fdisk") + 3];
	uint8_t param3 = console.keyboard_string[sizeof("fdisk") + 4];
	
	// Set the format size
	if ((param0 == '-') & (param1 == 's')) {
		
		if ((param2 > '0') & (param2 <= '9')) moduleLoaderFormat.format_size = param2 - '0';
		if (param2 == 'a') moduleLoaderFormat.format_size = 16;
		if (param2 == 'b') moduleLoaderFormat.format_size = 32;
		
		char in_prog_str[] = "Capacity ";
		console.print(in_prog_str, sizeof(in_prog_str));
		console.printInt(moduleLoaderFormat.format_size * 1024);
		console.printLn();
		
		return;
	}
	
	// Format a target device
	if ((param0 == '-') & (param1 == 'f')) {
		
		WrappedPointer pointer;
		uint8_t write_counter = 0;
		
		write_counter=0;
		
		if (((param2 < 'a') | (param2 > 'e')) & param2 != '/') {
			console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
			console.printLn();
			return;
		}
		
		console.print(in_prog_str, sizeof(in_prog_str));
		console.printLn();
		
		if (param2 == '/') {
			current_device = _VIRTUAL_STORAGE_ADDRESS__;
		} else {
			current_device = 0x40000 + (0x10000 * (param2 - 'a'));
		}
		
		for (uint32_t i=current_device; i < current_device + (moduleLoaderFormat.format_size * 1024); i++) {
			
			uint8_t byte = 0x20; // Sector state flag
			uint8_t flag = 0x00; // Sector data byte
			
			if (write_counter == 0) {
				fs.write(i, flag);
			} else {
				fs.write(i, byte);
			}
			
			// 32 byte page counter
			if (write_counter >= 31) {write_counter=0; _delay_ms(5);} else {write_counter++;}
			
		}
		
		// Initiate first sector
		char header[] = {0x13,'s','t','o','r','a','g','e',' ',' ',  0x00,0x00,0x00,0x00,  0x00,0x00,0x00,0x00};
		write_counter=0;
		
		for (uint8_t i=0; i < sizeof(header); i++) {
			
			fs.write(i + current_device, header[i]); eeprom_wait_state();
			
		}
		
		return;
	}
	
	return;
}


