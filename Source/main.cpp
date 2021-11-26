#include "main.h"

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
	
	// Launch the keyboard handler service
	const char keyboardTask[] = "kbsrv";
	scheduler.createTask(keyboardTask, sizeof(keyboardTask), keyboard_event_handler, 1000);
	
	//TaskPtr& task = scheduler.findTask(keyboardTask, sizeof(keyboardTask)-3);
	//scheduler.killTask(task);
	
	
	
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


