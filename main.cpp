#include "main.h"


int main(void) {
	
#ifndef __CORE_LIGHTWEIGHT_
	BusZero();
#endif
	
	// Initiate drivers
	extern_initiate();
	
	// Fire up the scheduler
	scheduler_start(80);
	
	while(1) asm("nop");
	
	scheduler_stop();
	
	extern_shutdown();
	
	return 1;
}


