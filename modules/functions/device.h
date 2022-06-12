//
// Device command

void command_drv(void);

struct ModuleLoaderDrv {
	ModuleLoaderDrv() {
		load_module("device", sizeof("device"), command_drv);
	}
}static loadModuleDrv;

void command_drv(void) {
	
	Device consoleDriver;
	if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	
	// List devices
	for (uint8_t i=0; i<_DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		for (uint8_t a=0; a < _DRIVER_NAME_LENGTH_MAX__; a++) {
			
			uint8_t nameChar = (uint8_t)deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == 0x20) break;
			
			call_extern(consoleDriver, 0x00, nameChar); // Write char
			
		}
		
		call_extern(consoleDriver, 0x03); // space
		
	}
	call_extern(consoleDriver, 0x01); // New line
	
	return;
}

