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
	const char device_name[] = "shitty";
	kernel.loadLibrary(device_name, sizeof(device_name), &displayLibraryEntryPoint);
	
	FunctionPtr deviceDriverID = kernel.getFuncAddress("shitty", 1);
	
	deviceDriverID(0x05, NULL, NULL, NULL, NULL);
	
	
	
	kernel.run();
	
	return 0;
}


