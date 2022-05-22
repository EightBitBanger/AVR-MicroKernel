// Extended memory device driver interface

#ifndef _HIGH_MEMORY__
#define _HIGH_MEMORY__

// Kernel memory space
#define _KERNEL_SIZE__          0x000ff   // Bytes reserved for the kernel

#define _KERNEL_BEGIN__         0x00000
#define _KERNEL_END__           (_KERNEL_BEGIN__ + _KERNEL_SIZE__)

#define _KERNEL_FLAGS__         _KERNEL_BEGIN__
#define _KERNEL_STACK_COUNT__   (_KERNEL_BEGIN__ + 0x000fc) // Memory address to the byte counter in the external stack

// Kernel State flags
#define _KERNEL_STATE_NORMAL__          0x00
#define _KERNEL_STATE_OUT_OF_MEMORY__   0xa0
#define _KERNEL_STATE_SEG_FAULT__       0xff


#include <avr/interrupt.h>

void ExtendedMemoryDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);


struct ExtendedMemoryDriver {
	
	Bus device_bus;
	
	uint32_t _STACK_BEGIN__;
	uint32_t _STACK_END__;
	
	uint32_t currentAddress;
	uint32_t returnAddress;
	
	ExtendedMemoryDriver() {loadLibrary(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), (DriverEntryPoint)ExtendedMemoryDeviceDriverEntryPoint);}
	
	void initiate(void) {
		
		char byte;
		
		device_bus.waitStateRead  = 2;
		device_bus.waitStateWrite = 0;
		
		// Check available memory
		for (_STACK_END__=0x00000; _STACK_END__ < 0x40000; _STACK_END__++) {
			
			device_bus.write(_STACK_END__, 0x55);
			device_bus.read(_STACK_END__, byte);
			
			if (byte != 0x55) break;
		}
		
		// Zero kernel space
		mem_zero(_KERNEL_BEGIN__, _KERNEL_END__);
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {device_bus.write(address, byte); return;}
	void read(uint32_t address, char& byte) {device_bus.read(address, byte); return;}
	
	
	uint32_t stack_alloc(uint32_t size) {
		
		WrappedPointer numberOfAllocations;
		for (uint8_t i=0; i<4; i++) device_bus.read(_KERNEL_STACK_COUNT__ + i, numberOfAllocations.byte[i]);
		
		// Check out of memory
		if ((numberOfAllocations.address + size + _STACK_BEGIN__) > _STACK_END__) {
			device_bus.write(_KERNEL_FLAGS__, _KERNEL_STATE_OUT_OF_MEMORY__);
			return nullptr;
		}
		
		// Get the beginning address pointing to the new allocation
		uint32_t new_pointer = _STACK_BEGIN__ + (numberOfAllocations.address);
		
		numberOfAllocations.address += size;
		
		for (uint8_t i=0; i<4; i++) device_bus.write(_KERNEL_STACK_COUNT__ + i, numberOfAllocations.byte[i]);
		
		return new_pointer;
	}
	
	void stack_free(uint32_t size) {
		
		WrappedPointer numberOfAllocations;
		for (uint8_t i=0; i<4; i++) device_bus.read(_KERNEL_STACK_COUNT__ + i, numberOfAllocations.byte[i]);
		
		numberOfAllocations.address -= size;
		
		for (uint8_t i=0; i<4; i++) device_bus.write(_KERNEL_STACK_COUNT__ + i, numberOfAllocations.byte[i]);
		
		return;
	}
	
	void mem_zero(uint32_t address_pointer, uint32_t size) {
		for (uint32_t i=0; i<=size; i++) {device_bus.write(address_pointer+i, 0x00);}
	}
	
	uint32_t stack_size(void) {
		
		WrappedPointer numberOfAllocations;
		for (uint8_t i=0; i<4; i++) device_bus.read(_KERNEL_STACK_COUNT__ + i, numberOfAllocations.byte[i]);
		
		return numberOfAllocations.address;
	}
	
} extendedMemoryDriver;




// Driver entry point
void ExtendedMemoryDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	WrappedPointer pointer;
	
	switch(functionCall) {
		
		case _DRIVER_INITIATE__: {extendedMemoryDriver.initiate(); break;}
		case _DRIVER_SHUTDOWN__: {extendedMemoryDriver.shutdown(); break;}
		
		case 0x00:  extendedMemoryDriver.returnAddress = extendedMemoryDriver.stack_alloc(extendedMemoryDriver.currentAddress); break;
		case 0x01:  extendedMemoryDriver.stack_free(extendedMemoryDriver.currentAddress); break;
		case 0x02:  extendedMemoryDriver.mem_zero(extendedMemoryDriver.currentAddress, paramA); break;
		case 0x03:  extendedMemoryDriver.returnAddress = extendedMemoryDriver.stack_size(); break;
		case 0x04:  extendedMemoryDriver.returnAddress = extendedMemoryDriver._STACK_END__; break;
		
		
		// Set the address pointer
		case 0x0a: 
			pointer.byte_t[0] = paramA;
			pointer.byte_t[1] = paramB;
			pointer.byte_t[2] = paramC;
			pointer.byte_t[3] = paramD;
			extendedMemoryDriver.currentAddress = pointer.address;
			break;
		
		// Get the address pointer
		case 0x0b: 
			pointer.address = extendedMemoryDriver.currentAddress;
			paramA = pointer.byte_t[0];
			paramB = pointer.byte_t[1];
			paramC = pointer.byte_t[2];
			paramD = pointer.byte_t[3];
			break;
		
		// Get the return pointer
		case 0x0c: 
			pointer.address = extendedMemoryDriver.returnAddress;
			paramA = pointer.byte_t[0];
			paramB = pointer.byte_t[1];
			paramC = pointer.byte_t[2];
			paramD = pointer.byte_t[3];
			break;
		
		default: break;
	}
	
	return;
}





#endif

