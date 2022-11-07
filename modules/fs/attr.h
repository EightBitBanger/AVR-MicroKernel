#ifndef _ATTRIBUTE_FUNCTION__
#define _ATTRIBUTE_FUNCTION__

void command_attrib(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	char filename[32];
	for (uint8_t i=0; i < 32; i++)
		filename[i] = 0x20;
	
	// Get file name from keyboard string
	uint8_t name_len=0;
	for (uint8_t a=0; a < 10; a++) {
		
		filename[a] = console.keyboard_string[sizeof("attr") + a];
		
		if (filename[a] == 0x20) {
			name_len = (a + 1);
			break;
		}
	}
	
	// Get attribute parameters
	char hex_string[2] = {'0', '0'};
	uint8_t char_a = console.keyboard_string[sizeof("attr") + name_len];
	uint8_t char_b = console.keyboard_string[sizeof("attr") + name_len + 1];
	
	// Check the volume header of the current device
	uint32_t current_device = set_device_scope();
	
	if (fs.device_check_header(current_device - SECTOR_SIZE) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	uint8_t attribute[4];
	char byte;
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		// Get current filename
		char current_file_name[16];
		for (uint32_t a=0; a < 16; a++)
			current_file_name[a] = 0x20;
		
		for (uint32_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, filename, 10) == 1) {
			
			// Read current file attributes
			for (uint8_t a=0; a < 4; a++) {
				fs.read(i + a + 15, (char&)attribute[a]);
				if (attribute[a] < 0x61) attribute[a] = 0x20;
			}
			
			// Add attribute
			if (char_a == '+') {
				
				// Execute | read | write attributes
				if (char_b == 'x') {attribute[0] = char_b;}
				if (char_b == 'r') {attribute[1] = char_b;}
				if (char_b == 'w') {attribute[2] = char_b;}
				
				if ((char_b != 'x') & (char_b != 'r') & (char_b != 'w')) {
					attribute[3] = char_b;
				}
				
				for (uint8_t a=0; a < 4; a++) {
					fs.write(i + a + 15, (char&)attribute[a]); eeprom_wait_state();
				}
				
			}
			
			// Remove attribute
			if (char_a == '-') {
				
				// Execute | read | write attributes
				if (char_b == 'x') {attribute[0] = 0x20;}
				if (char_b == 'r') {attribute[1] = 0x20;}
				if (char_b == 'w') {attribute[2] = 0x20;}
				
				for (uint8_t a=0; a < 4; a++) {
					fs.write(i + a + 15, (char&)attribute[a]); eeprom_wait_state();
				}
				
			}
			
		}
		
	}
	
	// Display current attributes
	for (uint8_t a=0; a < 4; a++)
		console.printChar( attribute[a] );
	
	console.printLn();
	
	return;
}

#endif

