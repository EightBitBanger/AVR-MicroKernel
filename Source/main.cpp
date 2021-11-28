#include "configuration.h"

#include <avr/io.h>
#include <util/delay.h>

#include "hardware_abstraction.h"

#include "kernel/kernel.h"

int main(void) {
	
	initiate_board;
	
	// Initiate the interface to hardware devices
	initiateDeviceIndex();
	
	_STACK_END__ = allocateSystemMemory();
	
	console.initiate();
	
	kernel.initiate();
	
	kernel.run();
	
	kernel.shutdown();
	
	while(1) nop;
	return 0;
}


