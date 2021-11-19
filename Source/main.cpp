
#include "main.h"


uint32_t allocate_system_memory(void);

int main(void) {
	
	initiate_board;
	
	// Initiate device index
	device.initiate();
	
	// Check available system memory
	_HEAP_END__ = allocate_system_memory();
	
	// Initiate and fire up the kernel
	kernel.initiate();
	kernel.run();
	
	return 0;
}


