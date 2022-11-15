//
// Device command

char msg_device_enabled[]      = "Device enabled";
char msg_device_disabled[]     = "Device disabled";
char error_driver_error[]      = "Device driver error";
char error_device_not_found[]  = "Device not found";

void command_device(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device consoleDriver;
	consoleDriver = (Device)get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__));
	if (consoleDriver == 0) return;
	
	uint8_t param0  = console.keyboard_string[sizeof("dev")];
	uint8_t param1  = console.keyboard_string[sizeof("dev") + 1];
	uint8_t param2  = console.keyboard_string[sizeof("dev") + 3];
	
	uint8_t page_counter=0;
	
	if (param0 == '-') {
		
		if (param1 == 'l') {
			
			for (uint8_t i=0; i<DEVICE_TABLE_SIZE; i++) {
				
				if (device_table.name[i][0] == 0x20) continue;
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < DEVICE_NAME_LENGTH_MAX; a++) {
					uint8_t nameChar = (uint8_t)device_table.name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
				
				// Page pause
				if (page_counter > 2) {page_counter = 0;
					call_extern(consoleDriver, 0x0d);
				}
				
			}
		}
		
		// Query a device slot
		if (param1 == 'q') {
			
			Bus device_bus;
			HardwareInformation hinfo;
			
			// Slot query selection
			if ((param2 >= '1') & (param2 <= '5')) {
				
				uint32_t address = 0x30000 + ((param2 - '0') * 0x10000);
				
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
		
		
		
		
		
	}
	
	return;
	
	
	
	
	
	
	
	
	
	
	
	
	// List devices
	if (param0 == 'l') {
		
		for (uint8_t i=0; i<DEVICE_TABLE_SIZE; i++) {
			
			if (device_table.name[i][0] == 0x20) continue;
			
			// List all devices
			if (param1 == 0x20) {
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < DEVICE_NAME_LENGTH_MAX; a++) {
					uint8_t nameChar = (uint8_t)device_table.name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
			}
			
			// List device drivers
			if (param1 == 'd') {
				
				if (device_table.type[i] != DEVICE_TYPE_DRIVER) continue;
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < DEVICE_NAME_LENGTH_MAX; a++) {
					uint8_t nameChar = (uint8_t)device_table.name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
			}
			
			// List device libraries
			if (param1 == 'l') {
				
				if (device_table.type[i] != DEVICE_TYPE_LIBRARY) continue;
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < DEVICE_NAME_LENGTH_MAX; a++) {
					uint8_t nameChar = (uint8_t)device_table.name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
			}
			
			// List device modules
			if (param1 == 'm') {
				
				if (device_table.type[i] != DEVICE_TYPE_MODULE) continue;
				
				// Print index
				console.printInt(i);
				console.printSpace();
				
				// Print name
				for (uint8_t a=0; a < DEVICE_NAME_LENGTH_MAX; a++) {
					uint8_t nameChar = (uint8_t)device_table.name[i][a];
					if (nameChar == 0x20) break;
					call_extern(consoleDriver, 0x00, nameChar); // Write char
				}
				
				page_counter++;
				call_extern(consoleDriver, 0x01); // New line
			}
			
			// Page pause
			if (page_counter > 2) {page_counter = 0;
				call_extern(consoleDriver, 0x0d);
			}
			
		}
		
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

