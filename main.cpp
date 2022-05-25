#include "main.h"


int main(void) {
	
	// Initiate device drivers
	extern_initiate();
	
	// Fire up the scheduler
	scheduler_start(80);
	
	while(1) asm("nop");
	
	scheduler_stop();
	
	extern_shutdown();
	
	return 1;
}


