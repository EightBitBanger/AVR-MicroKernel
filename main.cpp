//
// Kernel entry point

#include "main.h"

int main(void) {
	
	// Zero the external system bus and allow the system to stabilize
	address_zero();
	bus_zero();
	
	_delay_ms(1000);
	
	// Initiate device drivers
#ifdef __CORE_MAIN_
	__extern_call_init();
#endif
	
	// Attempt to identify devices on the system bus
#ifdef __HARDWARE_AUTO_DETECT_
	__detect_hardware(0x40000, 0x10000, 5);
#endif
	
#ifdef __CORE_MAIN_
	__kernel_initiate();
#endif
	
	// Fire up the scheduler with a counter base rate of 20
#ifdef __CORE_SCHEDULER_
	__scheduler_start(20);
#endif
	
	while(1) 
		asm("nop");
	
#ifdef __CORE_SCHEDULER_
	__scheduler_stop();
#endif
	
#ifdef __CORE_MAIN_
	__extern_call_shutdown();
#endif
	
	return 1;
}


