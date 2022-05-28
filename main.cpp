#include "main.h"


int main(void) {
	
	bus_zero();
	
	// Initiate device drivers
	__extern_call_init();
	
	
	char byte;
	for (uint8_t i=0; i < 10; i++) {
		
		displayDriver.read(i, byte);
		
		displayDriver.writeCharMask(byte, 0, i + 3);
		
	}
	
	
	
	// Fire up the scheduler
	__scheduler_start(20);
	
	while(1) 
		asm("nop");
	
	__scheduler_stop();
	
	__extern_call_shutdown();
	
	return 1;
}


