#include "main.h"

void testTaskFunc(void) {
	
	return;
}


int main(void) {
	
	initiate_board;
	
	kernel.initiateDeviceIndex();
	
	loadLibrary(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), &keyboardDeviceDriverEntryPoint);
	loadLibrary(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), &DisplayDeviceDriverEntryPoint);
	
	loadModule(command_memory,       "mem",  4);
	loadModule(command_clear_screen, "cls",  4);
	loadModule(command_device,       "dev",  4);
	loadModule(command_task,         "task", 5);
	loadModule(command_kill,         "kill", 5);
	
	_STACK_END__ = kernel.allocateSystemMemory();
	
	console.initiate();
	
	kernel.initiate();
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


