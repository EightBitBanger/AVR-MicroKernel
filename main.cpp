#include "main.h"


int main(void) {
	
	BusZero();
	
	extern_initiate();   // Initiate drivers
	scheduler_start(80); // Fire up the scheduler
	
	
	
	
	while(1) nop;
	
	scheduler_stop();
	extern_shutdown();
	
	return 1;
}


