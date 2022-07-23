//
// Device command

void command_drv(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "device"

struct ModuleLoaderDrv {
	ModuleLoaderDrv() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)command_drv, _DEVICE_TYPE_MODULE__);
	}
}static loadModuleDrv;

void command_drv(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device consoleDriver;
	if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	
	// Get the parameter from the keyboard string
	uint8_t param0  = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1  = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	
	// List devices
	if (param0 == 'l') {
		
		for (uint8_t i=0; i<_DEVICE_TABLE_SIZE__; i++) {
			
			if (deviceTable.device_name[i][0] == 0x20) continue;
			
			if (deviceTable.device_type[i] != _DEVICE_TYPE_DRIVER__) continue;
			
			for (uint8_t a=0; a < _DEVICE_NAME_LENGTH_MAX__; a++) {
				
				uint8_t nameChar = (uint8_t)deviceTable.device_name[i][a];
				if (nameChar == 0x20) break;
				
				call_extern(consoleDriver, 0x00, nameChar); // Write char
				
			}
			
			call_extern(consoleDriver, 0x03); // space
			
		}
		call_extern(consoleDriver, 0x01); // New line
		
		return;
	}
	
	//
	// No parameters - help anyone ?
	if (param0 == 0x20) {
		
		char help_line_a[] = "device l - list";
		char help_line_b[] = "device f n - free";
		
		console.print(help_line_a, sizeof(help_line_a)); console.printLn();
		console.print(help_line_b, sizeof(help_line_b)); console.printLn();
		//console.print(help_line_c, sizeof(help_line_c)); console.printLn();
		
		return;
	}
	
	return;
}

#undef __MODULE_NAME_
