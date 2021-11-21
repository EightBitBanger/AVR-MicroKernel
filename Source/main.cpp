#include "main.h"

int main(void) {
	
	initiate_board;
	
	// Initiate device map
	device.initiate();
	
	// Check available extended memory
	_STACK_END__ = allocate_system_memory();
	
	// Initiate and fire up the kernel
	kernel.initiate();
	
	//
	// Load the driver
	//kernel.driver.loadLibrary(&displayLibraryEntryPoint, "con");
	
	uint8_t deviceDriverID = kernel.driver.getFuncAddress("con");
	
	//if (deviceDriverID != 0) kernel.driver.callExtern(deviceDriverID, 0x00, 0x00);
	uint8_t returnValue=0;
	kernel.driver.callExtern(deviceDriverID, 0x05, returnValue);
	
	if (returnValue == 100)	kernel.run();
	while(1) nop;
	
	return 0;
}


