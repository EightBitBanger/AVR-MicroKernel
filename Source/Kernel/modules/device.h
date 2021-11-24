//
// Device command

void command_device(void);

struct ModuleDeviceCommand {
	
	ModuleDeviceCommand() {
		
		const char command_name[] = "device";
		loadModule(&command_device, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleDeviceCommand moduleDeviceCommand;

void command_device(void) {
	
	// List devices
	for (uint8_t i=0; i<_DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		for (uint8_t a=0; a < 8; a++) {
			
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			
			console.print(nameChar);
		}
		
		console.printLn();
	}
	
	return;
}

