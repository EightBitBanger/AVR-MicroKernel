#include "main.h"

void testTaskFunc(void) {
	
	return;
}


int main(void) {
	
	initiate_board;
	
	kernel.initiateDeviceIndex();
	
	_STACK_END__ = kernel.allocateSystemMemory();
	
	console.initiate();
	
	kernel.initiate();
	
	kernel.run();
	
	//kernel.shutdown();
	
	while(1) nop;
	return 0;
}


