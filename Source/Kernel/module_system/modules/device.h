//
// Device command

void command_device(void) {
	
	// List devices
	for (uint8_t i=0; i<_DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		for (uint8_t a=0; a < _DRIVER_TABLE_NAME_SIZE__; a++) {
			
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			
			console.printChar(nameChar);
		}
		
		console.printLn();
	}
	
	return;
}

