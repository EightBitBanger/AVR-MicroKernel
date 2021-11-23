#include "main.h"

int main(void) {
	
	initiate_board;
	
	// Initiate device map
	device.initiate();
	
	// Check available extended memory
	_STACK_END__ = allocate_system_memory();
	
	// Initiate and fire up the kernel
	kernel.initiate();
	
	// Call a function from a library example
	//const char name[] = "con";                                       // Display device name
	//loadLibrary(name, sizeof(name), &DisplayDeviceDriverEntryPoint); // Load the display driver
	//FunctionPtr funcPtr = getFuncAddress(name, sizeof(name));        // Get the address to the library entry point
	//callExtern(funcPtr, 0x04);                                       // Clear the display buffer
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


