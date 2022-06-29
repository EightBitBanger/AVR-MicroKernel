//
// Device command

void command_drv(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "device"

struct ModuleLoaderDrv {
	ModuleLoaderDrv() {
		load_library(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)command_drv, _DEVICE_TYPE_MODULE__);
	}
}static loadModuleDrv;
#undef __MODULE_NAME_

void command_drv(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device consoleDriver;
	if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	
	// List devices
	for (uint8_t i=0; i<_DEVICE_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.device_name[i][0] == 0x20) continue;
		
		for (uint8_t a=0; a < _DEVICE_NAME_LENGTH_MAX__; a++) {
			
			uint8_t nameChar = (uint8_t)deviceDriverTable.device_name[i][a];
			if (nameChar == 0x20) break;
			
			call_extern(consoleDriver, 0x00, nameChar); // Write char
			
		}
		
		call_extern(consoleDriver, 0x03); // space
		
	}
	call_extern(consoleDriver, 0x01); // New line
	
	return;
}

