#include "main.h"

void testTask(void) {
	
	//for (uint32_t i=0; i < 0xff; i++) kernel[i] = 0xff;
	
}



int main(void) {
	
	initiate_board;
	
	kernel.initiateDeviceIndex();
	
	_STACK_END__ = kernel.allocateSystemMemory();
	
	console.initiate();
	
	kernel.initiate();
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


