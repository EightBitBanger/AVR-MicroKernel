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
	kernel.driver.callExtern(deviceDriverID, 0x00, 0x00);
	
	kernel.run();
	
	return 0;
}


