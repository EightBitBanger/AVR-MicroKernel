//
// Device command

void command_drv(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "device"

char msg_device_enabled[]      = "Device enabled";
char msg_device_disabled[]     = "Device disabled";
char error_driver_error[]      = "Device driver error";
char error_device_not_found[]  = "Device not found";

struct ModuleLoaderDrv {
	ModuleLoaderDrv() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)command_drv, _DEVICE_TYPE_MODULE__);
	}
}static loadModuleDrv;

void command_drv(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device consoleDriver;
	if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	
	uint8_t param0  = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1  = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	
	uint8_t page_counter=0;
	
	// List devices
	if (param0 == 'l') {
		
		for (uint8_t i=0; i<_DEVICE_TABLE_SIZE__; i++) {
			
			if (deviceTable.device_name[i][0] == 0x20) continue;
			
			// List all devices
			if (param1 == 0x20) {
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < _DEVICE_NAME_LENGTH_MAX__; a++) {
					uint8_t nameChar = (uint8_t)deviceTable.device_name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
			}
			
			// List device drivers
			if (param1 == 'd') {
				
				if (deviceTable.device_type[i] != _DEVICE_TYPE_DRIVER__) continue;
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < _DEVICE_NAME_LENGTH_MAX__; a++) {
					uint8_t nameChar = (uint8_t)deviceTable.device_name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
			}
			
			// List device libraries
			if (param1 == 'l') {
				
				if (deviceTable.device_type[i] != _DEVICE_TYPE_LIBRARY__) continue;
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < _DEVICE_NAME_LENGTH_MAX__; a++) {
					uint8_t nameChar = (uint8_t)deviceTable.device_name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
			}
			
			// List device modules
			if (param1 == 'm') {
				
				if (deviceTable.device_type[i] != _DEVICE_TYPE_MODULE__) continue;
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < _DEVICE_NAME_LENGTH_MAX__; a++) {
					uint8_t nameChar = (uint8_t)deviceTable.device_name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
			}
			
			// Page pause
			if (page_counter > 2) {page_counter = 0;
				
				// Link to the keyboard driver
				Device keyboard_device;
				if (get_func_address(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), keyboard_device) == 0) return;
				
				console.print(msg_press_anykey, sizeof(msg_press_anykey));
				console.updateCursorPosition();
				
				uint8_t currentChar = 0x00;
				uint8_t lastChar    = 0x00;
				call_extern(keyboard_device, 0x00, lastChar);
				currentChar = lastChar;
				console.lastChar = lastChar;
				
				while(1) {
					call_extern(keyboard_device, 0x00, currentChar);
					if (lastChar != currentChar) break;
				}
				
				call_extern(consoleDriver, 0x01); // New line
			}
			
		}
		
		return;
	}
	
	// Query a device slot
	if (param0 == 'q') {
		
		Bus device_bus;
		HardwareInformation hinfo;
		
		// Slot query selection
		if ((param1 >= '1') & (param1 <= '5')) {
			
			uint32_t address = 0x30000 + ((param1 - '0') * 0x10000);
			
			device_bus.waitstate_write = 20;
			device_bus.waitstate_read  = 20;
			
			get_hardware_info(address, device_bus, hinfo);
			
			console.print(hinfo.device_name, 10);
			console.printLn();
			return;
		}
		
		console.print(error_device_not_found, sizeof(error_device_not_found));
		console.printLn();
		return;
	}
	
	//
	// No parameters - help anyone ?
	if (param0 == 0x20) {
		
		char help_line_a[] = "device l dlm - list";
		char help_line_b[] = "device f n - free";
		
		console.print(help_line_a, sizeof(help_line_a)); console.printLn();
		console.print(help_line_b, sizeof(help_line_b)); console.printLn();
		//console.print(help_line_c, sizeof(help_line_c)); console.printLn();
		
		return;
	}
	
	return;
}

#undef __MODULE_NAME_
