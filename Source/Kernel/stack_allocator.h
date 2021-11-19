//
// Stack allocator

#define  _STACK_BEGIN__   _KERNEL_END__  // After kernel memory
uint32_t _STACK_END__    =0;             // Total extended memory determined at boot

// Memory address to the number of allocated chunks
#define  _ALLOCATOR_COUNTER_ADDRESS__  0x000fc

void memory_read(uint32_t address, char& buffer);
void memory_write(uint32_t address, char byte);
void device_read(uint32_t address, char& byte);
void device_write(uint32_t address, char byte);

#include "cache.h"

#define nullptr 0x00000000

union Pointer {
	
	char byte[4];
	uint16_t word[2];
	uint32_t address;
	
	Pointer() {address=0;}
	Pointer(uint32_t newAddress) {address=newAddress;}
	
};

// Allocate stack memory and return a pointer to the first byte in this allocation
uint32_t stack_alloc(uint32_t size) {
	
	Pointer numberOfAllocations;
	
	// Increment the allocation counter
	for (uint8_t i=0; i<4; i++) numberOfAllocations.byte[i] = memory_cache[_ALLOCATOR_COUNTER_ADDRESS__ + i];
	
	// Check memory limit
	if ((_STACK_BEGIN__ + (numberOfAllocations.address + size)) >= _STACK_END__) {
		memory_write(_KERNEL_FLAGS__, _KERNEL_STATE_OUT_OF_MEMORY__);
		return nullptr;
	}
	
	// Get the beginning address pointing to the new allocation
	uint32_t new_pointer = _STACK_BEGIN__ + (numberOfAllocations.address);
	
	numberOfAllocations.address += size;
	for (uint8_t i=0; i<4; i++) memory_cache[_ALLOCATOR_COUNTER_ADDRESS__ + i] = numberOfAllocations.byte[i];
	
	return new_pointer;
}

// Free the last chunk of allocated memory
uint8_t stack_free(uint32_t size) {
	
	Pointer numberOfAllocations;
	for (uint8_t i=0; i<4; i++) numberOfAllocations.byte[i] = memory_cache[_ALLOCATOR_COUNTER_ADDRESS__ + i];
	
	if (numberOfAllocations.address >= size) {numberOfAllocations.address -= size;
		for (uint8_t i=0; i<4; i++) memory_cache[_ALLOCATOR_COUNTER_ADDRESS__ + i] = numberOfAllocations.byte[i];
		return 1;
	}
	
	return 0;
}

// Zero a section of memory
void mem_zero(uint32_t address_pointer, uint32_t size) {
	for (uint32_t i=0; i<size; i++) {memory_write(address_pointer+i, 0x00);}
}

// Return number of allocated bytes
uint32_t stack_count(void) {
	
	Pointer numberOfAllocations;
	for (uint8_t i=0; i<4; i++) numberOfAllocations.byte[i] = memory_cache[_ALLOCATOR_COUNTER_ADDRESS__ + i];
	
	return numberOfAllocations.address;
}

//
// Lowest level memory/device IO
void memory_read(uint32_t address, char& buffer) {
	
	// Address the device
	_BUS_LOWER_DIR__ = 0xff;
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_LOWER_OUT__ = (address & 0xff);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_UPPER_OUT__ = (address >> 16);
	_CONTROL_OUT__ = 0b00110100;
	
	// Set data direction
	_BUS_LOWER_DIR__ = 0x00; // Data bus input
	_BUS_LOWER_OUT__ = 0xff; // Pull-up resistors
	
	_CONTROL_OUT__ = 0b00100100;
	
	// Wait states
#ifdef _MEMORY_READ_WAIT_STATE_1__
	nop;
#endif
#ifdef _MEMORY_READ_WAIT_STATE_2__
	nop; nop;
#endif
#ifdef _MEMORY_READ_WAIT_STATE_3__
	nop; nop; nop;
#endif
	
	// Read the data byte
	buffer = _BUS_LOWER_IN__;
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_UPPER_OUT__ = 0x0f;
	
	return;
}

void memory_write(uint32_t address, char byte) {
	
	// Address the device
	_BUS_LOWER_DIR__ = 0xff;
	
	_CONTROL_OUT__ = 0b00111111;
	//_CONTROL_OUT__ |= (1 << _CONTROL_ADDRESS_LATCH__);
	_BUS_LOWER_OUT__ = (address & 0xff);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_UPPER_OUT__ = (address >> 16);
	//_CONTROL_OUT__ &= ~(1 << _CONTROL_ADDRESS_LATCH__);
	_CONTROL_OUT__ = 0b00110101;
	
	// Cast the data byte
	_BUS_LOWER_OUT__ = byte;
	
	_CONTROL_OUT__ = 0b00010101;
	
	// Wait states
#ifdef _MEMORY_WRITE_WAIT_STATE_1__
	nop;
#endif
#ifdef _MEMORY_WRITE_WAIT_STATE_2__
	nop; nop;
#endif
#ifdef _MEMORY_WRITE_WAIT_STATE_3__
	nop; nop; nop;
#endif
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_UPPER_OUT__ = 0x0f;
	
	return;
}

void device_read(uint32_t address, char& byte) {
	
	// Address the device
	_BUS_LOWER_DIR__ = 0xff;
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_LOWER_OUT__ = (address & 0xff);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_UPPER_OUT__ = (address >> 16);
	_CONTROL_OUT__ = 0b00110100;
	
	// Set data direction
	_BUS_LOWER_DIR__ = 0x00; // Data bus input
	_BUS_LOWER_OUT__ = 0xff; // Pull-up resistors
	
	// Read operation
	_CONTROL_OUT__ = 0b00100100;
	
	// Wait states
#ifdef _DEVICE_READ_WAIT_STATE_1__
	for (uint8_t i=0; i<10; i++) {nop;nop;nop;nop;}
#endif
#ifdef _DEVICE_READ_WAIT_STATE_2__
	for (uint8_t i=0; i<50; i++) {nop;nop;nop;nop;}
#endif
#ifdef _DEVICE_READ_WAIT_STATE_3__
	for (uint8_t i=0; i<100; i++) {nop;nop;nop;nop;}
#endif
	
	byte = _BUS_LOWER_IN__;
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_UPPER_OUT__ = 0x0f;
	
	return;
}

void device_write(uint32_t address, char byte) {
	
	// Address the device
	_BUS_LOWER_DIR__ = 0xff;
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_LOWER_OUT__ = (address & 0xff);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_UPPER_OUT__ = (address >> 16);
	_CONTROL_OUT__ = 0b00110101;
	
	_BUS_LOWER_OUT__ = byte;
	
	// Write operation
	_CONTROL_OUT__ = 0b00010101;
	
	// Wait states
#ifdef _DEVICE_WRITE_WAIT_STATE_1__
	for (uint8_t i=0; i<10; i++) {nop;nop;nop;nop;}
#endif
#ifdef _DEVICE_WRITE_WAIT_STATE_2__
	for (uint8_t i=0; i<50; i++) {nop;nop;nop;nop;}
#endif
#ifdef _DEVICE_WRITE_WAIT_STATE_3__
	for (uint8_t i=0; i<100; i++) {nop;nop;nop;nop;}
#endif
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_UPPER_OUT__ = 0x0f;
	
	return;
}


