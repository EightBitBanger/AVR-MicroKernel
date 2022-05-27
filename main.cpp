#include "main.h"


int main(void) {
	
	// Initiate device drivers
	__extern_call_init();
	
	// Fire up the scheduler
	__scheduler_start(80);
	
	while(1) 
		asm("nop");
	
	__scheduler_stop();
	
	__extern_call_shutdown();
	
	return 1;
}


