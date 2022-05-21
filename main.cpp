#include "main.h"


int main(void) {
	
	BusZero();
	
	extern_initiate();
	scheduler_start(80);
	
	
	
	
	while(1) nop;
	
	scheduler_stop();
	extern_shutdown();
	
	return 1;
}


