#ifndef _RN_FUNCTION__
#define _RN_FUNCTION__

void command_rn(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint32_t current_device = set_device_scope();
	
	if (fs.device_check_header(current_device - SECTOR_SIZE) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	char filenameA[32]; // Source name
	char filenameB[32]; // New name
	
	for (uint8_t i=0; i < 32; i++) {
		filenameA[i] = 0x20;
		filenameB[i] = 0x20;
	}
	
	// Split keyboard string names
	uint8_t swtch=0;
	uint8_t i=0;
	for (uint8_t a=0; a < _MAX_KEYBOARD_STRING_LENGTH__ - sizeof("rn"); a++) {
		
		char str_char = console.keyboard_string[sizeof("rn") + a];
		
		if ((str_char == 0x20) & (swtch == 0)) {
			swtch = 1;
			i=0;
			continue;
		}
		
		if (swtch == 0) {
			filenameA[i] = str_char;
			i++;
		}
		
		if (swtch == 1) {
			if (str_char == 0x20) break;
			filenameB[i] = str_char;
			i++;
		}
		
	}
	
	
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + DEVICE_CAPACITY;
	
	char byte;
	
	for (uint32_t i=device_start; i < device_end; i += SECTOR_SIZE) {
		
		fs.read(i, byte);
		
		if (byte != 0x55) continue; // Only file header sectors
		
		// Get current filename
		char current_file_name[10];
		for (uint8_t a=0; a < 10; a++)
			current_file_name[a] = 0x20;
		
		for (uint8_t a=0; a < 10; a++)
			fs.read(i + a + 1, current_file_name[a]);
		
		// Compare filenames
		if (strcmp(current_file_name, filenameA, 10) == 1) {
			
			// Check if we can write to this file
			if (file_get_attribute(filenameA, 2) != 'w') {
				console.print(msg_file_write_protected, sizeof(msg_file_write_protected));
				console.printLn();
				return;
			}
			
			// Write new file name
			for (uint8_t a=0; a < 10; a++) {
				fs.write(i + a + 1, (uint8_t&)filenameB[a]); eeprom_wait_state();
			}
			
			return;
		}
		
	}
	
	console.print(msg_file_not_found, sizeof(msg_file_not_found));
	console.printLn();
	
	return;
}


#endif

