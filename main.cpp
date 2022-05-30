#include "main.h"


int main(void) {
	
	bus_zero();
	
	// Initiate device drivers
	__extern_call_init();
	
	// Detect available devices attached to the system bus
	__detect_hardware(0x40000, 0x10000, 5);
	
	// Fire up the scheduler
	__scheduler_start(20);
	
	while(1) 
		asm("nop");
	
	__scheduler_stop();
	
	__extern_call_shutdown();
	
	return 1;
}


