#include "main.h"

int main(void) {
	
	initiate_board;
	
	// Initiate device map
	device.initiate();
	
	// Check available extended memory
	_STACK_END__ = allocate_system_memory();
	
	// Setup the command console
	console.initiate();
	
	// Initiate and fire up the kernel
	kernel.initiate();
	
	
	
	//const char deviceName[] = _DISPLAY_CONSOLE__;
	//FunctionPtr& displayDriverPtr = getFuncAddress(deviceName, sizeof(deviceName));
	
	//callExtern(displayDriverPtr, 0x04, null, null, null, null);
	
	/*
	
	// List devices
	for (uint8_t i=0; i<_DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < sizeof(deviceName); a++) {
			
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == deviceName[a]) count++; else break;
			
			if (count >= sizeof(deviceName)) {
				
				console.print(deviceName, sizeof(deviceName));
				
				callExtern(deviceDriverTable.driver_entrypoint_table[i], 0x04, null, null, null, null);
				
			}
			
		}
		
		console.printLn();
	}
	
	*/
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


