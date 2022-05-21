// System call routing functions

// Allocation functions
uint32_t stack_alloc(uint32_t size);
uint32_t stack_free(uint32_t size);


uint32_t stack_alloc(uint32_t size) {
	
	WrappedPointer pointer(size);
	DriverEntryPoint driverEntry;
	
	if (getFuncAddress(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), driverEntry) == 0) return nullptr;
	
	// Set requested size
	callExtern(driverEntry, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	// Request the memory
	callExtern(driverEntry, 0x00);
	// Get return result
	callExtern(driverEntry, 0x0c, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	return pointer.address;
}

uint32_t stack_free(uint32_t size) {
	
	WrappedPointer pointer(size);
	DriverEntryPoint driverEntry;
	
	if (getFuncAddress(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), driverEntry) == 0) return nullptr;
	
	// Set requested size
	callExtern(driverEntry, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	// Free the memory
	callExtern(driverEntry, 0x01);
	// Get return result
	callExtern(driverEntry, 0x0c, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	return pointer.address;
}


