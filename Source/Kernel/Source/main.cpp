#include "main.h"

int main(void) {
	
	initiate_board;
	
	kernel.initiateDeviceIndex();
	
	_STACK_END__ = kernel.allocateSystemMemory();
	
	console.initiate();
	kernel.initiate();
	
	// Launch the keyboard handler service
	const char keyboardTask[] = "kbsrv";
	scheduler.createTask(keyboardTask, sizeof(keyboardTask), keyboard_event_handler, 1000);
	
	// TESTING
	//TaskPtr& task = scheduler.findTask(keyboardTask, sizeof(keyboardTask));
	//scheduler.killTask(task);
	
	
	
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


