//
// External memory stack allocator

void memory_read(uint32_t address, char& buffer);
void memory_write(uint32_t address, char byte);
void device_read(uint32_t address, char& byte);
void device_write(uint32_t address, char byte);

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
	
	// Check segmentation fault
	if ((address < _DEVICE_ADDRESS_START__) || (address >= _DEVICE_ADDRESS_END__)) 
		return;
	
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
	
	// Check segmentation fault
	if ((address < _DEVICE_ADDRESS_START__) || (address >= _DEVICE_ADDRESS_END__))
	return;
	
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


