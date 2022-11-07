#ifndef _DIR_COMMAND__
#define _DIR_COMMAND__

void command_dir(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	// Check the volume header of the current device
	uint32_t current_device = set_device_scope();
	
	if (fs.device_check_header(current_device - SECTOR_SIZE) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
	uint8_t param0 = console.keyboard_string[sizeof("dir")];
	uint8_t param1 = console.keyboard_string[sizeof("dir") + 1];
	
	uint8_t pageflag = 0;
	if ((param0 == '-') & (param1 == 'p')) 
		pageflag = 1;
	
	// List contents of the current device
	call_extern(storageDevice, 0x2f, pageflag);
	
	return;
}

#endif

