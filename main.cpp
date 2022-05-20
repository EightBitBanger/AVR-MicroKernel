#include "main.h"

uint32_t stack_alloc(uint32_t size) {
	
	WrappedPointer pointer(size);
	
	DriverEntryPoint driverEntry;
	if (getFuncAddress("ExMem", 6, driverEntry) == 0) return 0;
	
	// Set requested size
	callExtern(driverEntry, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	// Request the memory
	callExtern(driverEntry, 0x00);
	
	callExtern(driverEntry, 0x0c, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	return pointer.address;
}


int main(void) {
	
	BusZero();
	
	extern_initiate();
	scheduler_start(80);
	
	while(1) nop;
	
	scheduler_stop();
	extern_shutdown();
	
	return 1;
}


