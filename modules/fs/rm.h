#ifndef _RM_FUNCTION__
#define _RM_FUNCTION__

void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint32_t current_device = fs_set_device_scope();
	
	if (device_check_header(_FILE_SYSTEM__, sizeof(_FILE_SYSTEM__), current_device) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	char filename[32];
	
	for (uint8_t i=0; i < 32; i++) 
		filename[i] = 0x20;
	
	for (uint8_t a=0; a < _MAX_KEYBOARD_STRING_LENGTH__ - sizeof("rm"); a++) {
		
		filename[a] = console.keyboard_string[sizeof("rm") + a];
	}
	
	if (file_delete(filename) == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
	}
	
	return;
}

#endif

