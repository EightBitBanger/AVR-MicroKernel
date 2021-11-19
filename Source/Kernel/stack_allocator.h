//
// Dynamic allocator

// Memory allocation range
#define  _HEAP_BEGIN__    0x00100
uint32_t _HEAP_END__     =0;           // Total memory determined at boot

// Memory address for the number of allocated chunks
#define  _ALLOCATOR_COUNTER_ADDRESS__  0x000fc

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
	if ((_HEAP_BEGIN__ + (numberOfAllocations.address + size)) >= _HEAP_END__) {
		memory_write(_KERNEL_FLAGS__, _KERNEL_STATE_OUT_OF_MEMORY__);
		return nullptr;
	}
	
	// Get the beginning address pointing to the new allocation
	uint32_t new_pointer = _HEAP_BEGIN__ + (numberOfAllocations.address);
	
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



