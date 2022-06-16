// Initiate hardware for the retroboard r644-20

#ifndef __INITIATE_BOARD_
#define __INITIATE_BOARD_

void initiate_board(void) {
	
	char skip=0;
	char byte=0;
	
	console.setCursorPosition(0, 0);
	console.printChar('0');
	
	// Allocate available external memory 0x00000 - 0x3ffff
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
	
}

#endif

