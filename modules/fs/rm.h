#ifndef _RM_FUNCTION__
#define _RM_FUNCTION__

void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	char filename[32];
	
	for (uint8_t i=0; i < 32; i++) 
		filename[i] = 0x20;
	
	for (uint8_t a=0; a < _MAX_KEYBOARD_STRING_LENGTH__ - sizeof("rm"); a++) {
		
		filename[a] = console.keyboard_string[sizeof("rm") + a];
	}
	
	if (fs.file_delete(filename) == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
	}
	
	return;
}

#endif

