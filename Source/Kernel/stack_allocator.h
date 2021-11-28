//
// External memory stack allocator

// Extended memory cache size
#define _CACHE_SIZE__  16

struct ExternalMemoryCache {
	
	uint32_t current_address;
	char cache[_CACHE_SIZE__];
	char protectionOverflowBuffer[1];
	
	// External memory cache interface
	char& operator[] (uint32_t new_pointer) {
		
		// Check segmentation fault
		if ((new_pointer < _USER_BEGIN__) || (new_pointer >= _USER_END__)) {
			memory_write(_KERNEL_FLAGS__, _KERNEL_STATE_SEG_FAULT__);
			return protectionOverflowBuffer[0];
		}
		
		// Check cache out of bounds
		if ((new_pointer < current_address) || (new_pointer >= (current_address + _CACHE_SIZE__))) {
			// Dump the cache back to memory
			for (uint8_t i=0; i<_CACHE_SIZE__; i++) {
				memory_write(current_address + i, cache[i]);
				memory_read(new_pointer + i, cache[i]);
			}
			current_address = new_pointer;
		}
		
		return cache[new_pointer - current_address];
	}
	
	ExternalMemoryCache() {
		
		current_address = _STACK_BEGIN__;
		
		for (uint8_t i=0; i<_CACHE_SIZE__; i++) {cache[i] = 0x00;}
		
	}
	
};
ExternalMemoryCache cache;

// Allocate stack memory and return a pointer to the first byte of the allocation
uint32_t stack_alloc(uint32_t size) {
	
	WrappedPointer numberOfAllocations;
	for (uint8_t i=0; i<4; i++) memory_read(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
	
	// Check memory segmentation
	if (_STACK_BEGIN__ + (numberOfAllocations.address + size) >= _USER_END__) {
		memory_write(_KERNEL_FLAGS__, _KERNEL_STATE_OUT_OF_MEMORY__);
		return nullptr;
	}
	
	// Get the beginning address pointing to the new allocation
	uint32_t new_pointer = _STACK_BEGIN__ + (numberOfAllocations.address);
	
	numberOfAllocations.address += size;
	for (uint8_t i=0; i<4; i++) memory_write(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
	
	return new_pointer;
}

// Free allocated memory of the given size
void stack_free(uint32_t size) {
	
	WrappedPointer numberOfAllocations;
	for (uint8_t i=0; i<4; i++) memory_read(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
	
	numberOfAllocations.address -= size;
	
	for (uint8_t i=0; i<4; i++) memory_write(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
	
	return;
}

// Zero a range of memory
void mem_zero(uint32_t address_pointer, uint32_t size) {
	for (uint32_t i=0; i<size; i++) {memory_write(address_pointer+i, 0x00);}
}

// Return the number of bytes allocated in the stack
uint32_t stack_size(void) {
	
	WrappedPointer numberOfAllocations;
	for (uint8_t i=0; i<4; i++) memory_read(_ALLOCATOR_COUNTER_ADDRESS__ + i, numberOfAllocations.byte[i]);
	
	return numberOfAllocations.address;
}

