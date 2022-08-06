//
// Kernel entry point

#include "main.h"

int main(void) {
	
	// Zero the external system bus
	address_zero();
	bus_zero();
	
	// Allow the system time to stabilize
	_delay_ms(300);
	
	// Initiate device drivers
#ifdef __CORE_MAIN_
	__extern_call_init();
#endif
	
	// Attempt to identify devices on the system bus
#ifdef __HARDWARE_AUTO_DETECT_
	__detect_hardware();
#endif
	
#ifdef __CORE_MAIN_
	__kernel_initiate();
#endif
	
#ifdef __CORE_SCHEDULER_
	
	// Fire up the scheduler
	__scheduler_start();
	
	while(1) asm("nop");
	
	__scheduler_stop();
	
#endif
	
#ifdef __CORE_MAIN_
	
	__extern_call_shutdown();
	
#endif
	
	return 1;
}


