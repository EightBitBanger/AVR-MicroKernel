#include "main.h"

int main(void) {
	
	initiate_board;
	
	// Initiate device map
	device.initiate();
	
	// Check available extended memory
	_STACK_END__ = allocate_system_memory();
	
	// Initiate and fire up the kernel
	kernel.initiate();
	
	//if (kernel.getFuncAddress("con", 3) == (FunctionPtr)nullfunction) while(1) nop;
	
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


