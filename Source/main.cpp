
#include "main.h"


uint32_t allocate_system_memory(void);

int main(void) {
	
	initiate_board;
	
	// Initiate kernel memory
	deviceHandler.initiate();
	
	// Check available system memory
	_HEAP_END__ = allocate_system_memory();
	
	// Initiate and fire up the kernel
	kernel.initiate();
	
	while(1) 
		kernel.run();
	
	return 0;
}




// Allocate and display total available system memory
uint32_t allocate_system_memory(void) {
	
	displayDriver.cursorSetPosition(1, 0);
	console.cursorLine = 1;
	console.cursorPos  = 0;
	
	uint32_t updateTimer=0;
	
	displayDriver.writeString(string_memory_allocator, sizeof(string_memory_allocator));
	
	string memoryString;
	
#ifdef _TEST_RAM_EXTENSIVELY__
	char readByteTest=0x00;
#endif
	
	char readByte=0x00;
	uint32_t total_system_memory;
	
	// Copy the kernel memory before wiping the ram
	uint8_t kernelBuffer[0x1f];
	uint8_t kernelBufferSize = 0x1f;
	for (uint8_t address=0; address < kernelBufferSize; address++) {
		memory_read(address, readByte);
		kernelBuffer[address] = readByte;
	}
	
	for (total_system_memory=0x00000; total_system_memory < 0x40000; ) {
		
		// Run some memory cycles
		memory_write(total_system_memory, 0x55);
		memory_read(total_system_memory, readByte);
		
		// Check if the byte(s) stuck
#ifdef _TEST_RAM_EXTENSIVELY__
		// Run some more memory cycles
		memory_write(total_system_memory, 0xaa);
		memory_read(total_system_memory, readByteTest);
		if ((readByte == 0x55) && (readByteTest == 0xaa)) {
#else
		if (readByte == 0x55) {
#endif
		
#ifdef _FAST_BOOT__
			total_system_memory += 0x8000;
#else
			total_system_memory++;
#endif
		} else {
			
			if (total_system_memory == 0x00000) {
				
				// Restore the kernel memory(if its installed)
				for (uint8_t address=0; address < kernelBufferSize; address++) memory_write(address, kernelBuffer[address]);
				
				displayDriver.writeString(error_exmem_not_installed, sizeof(error_exmem_not_installed));
				displayDriver.cursorSetPosition(2, 0);
				while(1) nop;
			}
			break;
		}
		
		// Restore the kernel memory
		if ((total_system_memory < 0x0001f) || (total_system_memory == 0x8000)) {
			for (uint8_t address=0; address < kernelBufferSize; address++) memory_write(address, kernelBuffer[address]);
		}
		
		
		// Update display
		if (updateTimer > 1024) {updateTimer=0;
			
			string memoryString;
			intToString(total_system_memory, memoryString.str);
			displayDriver.writeString(memoryString.str, 7);
			
		} else {updateTimer++;}
		
	}
	
	// Calculate free memory
	uint32_t memoryFree = (total_system_memory - 0x00100);
	
	// Display available memory
	intToString(memoryFree, memoryString.str);
	displayDriver.writeString(memoryString.str, 7);
	
	return total_system_memory;
}



