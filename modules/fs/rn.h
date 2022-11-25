#ifndef _RN_FUNCTION__
#define _RN_FUNCTION__

void command_rn(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint32_t current_device = fs_set_device_scope();
	
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	char filenameA[_MAX_KEYBOARD_STRING_LENGTH__]; // Source name
	char filenameB[_MAX_KEYBOARD_STRING_LENGTH__]; // New name
	
	for (uint8_t i=0; i < _MAX_KEYBOARD_STRING_LENGTH__; i++) {
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
	
	if (file_rename(filenameA, filenameB) == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
	}
	
	return;
}


#endif

