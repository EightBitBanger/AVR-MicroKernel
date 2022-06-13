//
// Kernel entry point

#include "main.h"


int main(void) {
	
	bus_zero();
	
	// Initiate device drivers
	__extern_call_init();
	
	// Detect devices attached to the system bus
#ifdef __HARDWARE_AUTO_DETECT_
	__detect_hardware(0x40000, 0x10000, 5);
#endif
	
	console.setCursorPosition(0, 0);
	console.printChar('0');
	
	char skip=0;
	char byte=0;
	
	// Allocate available external memory
	for (extendedMemoryDriver._STACK_END__=0x00000; extendedMemoryDriver._STACK_END__ < 0x40000; extendedMemoryDriver._STACK_END__++) {
		
		bus_write_byte(extendedMemoryDriver.device_bus, extendedMemoryDriver._STACK_END__, 0x55);
		bus_read_byte(extendedMemoryDriver.device_bus, extendedMemoryDriver._STACK_END__, byte);
		
		if (byte != 0x55) break;
		
		skip++;
		if (skip > 100) {skip=0;
			
			console.setCursorPosition(0, 0);
			console.printInt( extendedMemoryDriver._STACK_END__ );
			
		}
		
	}
	
	// Zero kernel memory
	extendedMemoryDriver.mem_zero(_KERNEL_BEGIN__, _KERNEL_END__);
	
	console.setCursorPosition(1, 0);
	console.printPrompt();
	
	// Speaker test
	speakerDriver.beep(75, 1);
	
	
	// Fire up the scheduler
	__scheduler_start(20);
	
	while(1) 
		asm("nop");
	
	__scheduler_stop();
	
	__extern_call_shutdown();
	
	return 1;
}


