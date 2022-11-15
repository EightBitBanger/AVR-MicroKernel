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
	for (name_len=0; name_len < 10; name_len++) {
		filename[name_len] = console.keyboard_string[sizeof("attr") + name_len];
		if (filename[name_len] == 0x20) 
			break;
	}name_len++;
	
	// Get attribute parameters
	uint8_t char_a = console.keyboard_string[sizeof("attr") + name_len];
	uint8_t char_b = console.keyboard_string[sizeof("attr") + name_len + 1];
	
	// Check the volume header of the current device
	uint32_t current_device = set_device_scope();
	
	if (device_check_header(_MASS_STORAGE__, current_device) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	// Check the file exists
	if (file_open(filename) == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
		return;
	}
	
	
	//
	// Execute | read | write | Extra
	
	// Add attribute
	if (char_a == '+') {
		
		if (char_b == 'x') file_set_attribute(filename, char_b, 0); eeprom_wait_state();
		if (char_b == 'r') file_set_attribute(filename, char_b, 1); eeprom_wait_state();
		if (char_b == 'w') file_set_attribute(filename, char_b, 2); eeprom_wait_state();
		
		if ((char_b != 'x') & (char_b != 'r') & (char_b != 'w'))
			file_set_attribute(filename, char_b, 3); eeprom_wait_state();
		
		return;
	}
	
	// Remove attribute
	if (char_a == '-') {
		uint8_t zerochar = 0x00;
		
		if (char_b == 'x') file_set_attribute(filename, zerochar, 0); eeprom_wait_state();
		if (char_b == 'r') file_set_attribute(filename, zerochar, 1); eeprom_wait_state();
		if (char_b == 'w') file_set_attribute(filename, zerochar, 2); eeprom_wait_state();
		
		if ((char_b != 'x') & (char_b != 'r') & (char_b != 'w'))
			file_set_attribute(filename, zerochar, 3); eeprom_wait_state();
		
		return;
	}
	
	return;
}

#endif