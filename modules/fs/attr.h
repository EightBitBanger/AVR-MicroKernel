#ifndef _ATTRIBUTE_FUNCTION__
#define _ATTRIBUTE_FUNCTION__

void command_attrib(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

void command_attrib(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	char filename[10];
	for (uint8_t i=0; i < 10; i++)
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
	
	
	// Open file
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, (uint8_t&)filename[a]);
	
	uint8_t return_value;
	call_extern(storageDevice, 0x0d, return_value);
	
	uint8_t blank_byte = 0x20;
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, blank_byte);
	
	if (return_value != 0) {
		
		console.printChar( char_a );
		console.printChar( char_b );
		
		
		// Add attribute
		if (char_a == '+') {
			
			// Execute | read | write attributes
			if (char_b == 'x') {fs.file_set_attribute(filename, 'x', 0);}
			if (char_b == 'r') {fs.file_set_attribute(filename, 'r', 1);}
			if (char_b == 'w') {fs.file_set_attribute(filename, 'w', 2);}
			
			// Extended attribute
			if ((char_b != 'x') & (char_b != 'r') & (char_b != 'w')) {
				uint8_t attribute;
				attribute = fs.file_get_attribute(filename, 3);
				
				if (attribute == 0x00) {
					fs.file_set_attribute(filename, char_b, 3);
				}
				
			}
			console.printLn();
			return;
		}
		
		// Remove attribute
		if (char_a == '-') {
			
			// Execute | read | write attributes
			if (char_b == 'x') {fs.file_set_attribute(filename, 0x00, 0);}
			if (char_b == 'r') {fs.file_set_attribute(filename, 0x00, 1);}
			if (char_b == 'w') {fs.file_set_attribute(filename, 0x00, 2);}
			
			// Extended attribute
			uint8_t attribute;
			attribute = fs.file_get_attribute(filename, 3);
			
			if (attribute == char_b) 
				fs.file_set_attribute(filename, 0x00, 3);
			
			console.printLn();
			return;
		}
		
		// Display current file attributes
		uint8_t attributes[4];
		
		for (uint8_t i=0; i<4; i++) {
			attributes[i] = fs.file_get_attribute(filename, i);
			if ((attributes[i] > 0x60) & (attributes[i] < 0x7b)) console.printChar( attributes[i] );
		}
		
		console.printLn();
		
	} else {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
	}
	
	
	return;
}

#endif

