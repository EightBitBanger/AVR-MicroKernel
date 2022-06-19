//
// Kernel entry point

#include "main.h"

int main(void) {
	
	// Allow the system to stabilize and zero the external system bus
	_delay_ms(800);
	
	bus_zero();
	
	// Initiate device drivers
	__extern_call_init();
	
	// Attempt to identify devices on the system bus
#ifdef __HARDWARE_AUTO_DETECT_
	__detect_hardware(0x40000, 0x10000, 5);
#endif
	
	initiate_kernel();
	
	// Fire up the scheduler with a counter base rate of 20
	__scheduler_start(20);
	
	while(1) 
		asm("nop");
	
	__scheduler_stop();
	
	__extern_call_shutdown();
	
	return 1;
}


