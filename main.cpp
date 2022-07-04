//
// Kernel entry point

#include "main.h"

int main(void) {
	
	// Zero the external system bus and allow the system to stabilize
	address_zero();
	bus_zero();
	
	_delay_ms(100);
	
	// Initiate device drivers
	__extern_call_init();
	
	// Attempt to identify devices on the system bus
#ifdef __HARDWARE_AUTO_DETECT_
	__detect_hardware(0x40000, 0x10000, 5);
#endif
	
	__kernel_initiate();
	
	// Fire up the scheduler with a counter base rate of 20
	__scheduler_start(1);
	
	while(1) 
		asm("nop");
	
	__scheduler_stop();
	
	__extern_call_shutdown();
	
	return 1;
}


