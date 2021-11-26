#include "main.h"

void task(void) {
	const char String[] = "task..";
	console.print(String, sizeof(String));
	return;
}


int main(void) {
	
	initiate_board;
	
	// Initiate device map
	kernel.initiateDeviceIndex();
	
	// Check available extended memory
	_STACK_END__ =  kernel.allocateSystemMemory();
	
	// Setup the command console
	console.initiate();
	
	// Initiate and fire up the kernel
	kernel.initiate();
	
	scheduler.createTask(keyboard_event_handler, 17000);
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


