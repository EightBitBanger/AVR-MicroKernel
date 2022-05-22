// System call routing functions

// Allocation
uint32_t stack_alloc(uint32_t size);
uint32_t stack_free(uint32_t size);

// File system
void fs_seek(uint32_t address); // Set seek location pointer
void fs_write(uint8_t byte);    // Write to location auto-incrementing the seek pointer
void fs_read(uint8_t& byte);    // Read from location auto-incrementing the seek pointer




//
// Allocation

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


//
// File system

void fs_write(uint8_t byte) {
	
	DriverEntryPoint storageDriverPtr;
	
	// Link to system storage
	if (getFuncAddress(_HARD_STORAGE__, sizeof(_HARD_STORAGE__), storageDriverPtr) == nullptr) return;
	
	callExtern(storageDriverPtr, 0x03, byte);
	
}

void fs_read(uint8_t& byte) {
	
	DriverEntryPoint storageDriverPtr;
	
	// Link to system storage
	if (getFuncAddress(_HARD_STORAGE__, sizeof(_HARD_STORAGE__), storageDriverPtr) == nullptr) return;
	
	callExtern(storageDriverPtr, 0x04, byte);
	
}

void fs_seek(uint32_t address) {
	
	WrappedPointer pointer;
	pointer.address = address;
	DriverEntryPoint storageDriverPtr;
	
	// Link to system storage
	if (getFuncAddress(_HARD_STORAGE__, sizeof(_HARD_STORAGE__), storageDriverPtr) == nullptr) return;
	
	callExtern(storageDriverPtr, 0x00, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
}


