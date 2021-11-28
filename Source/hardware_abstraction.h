//
// Hardware abstraction layer

// Root level system IO
void memory_read(uint32_t address, char& buffer);
void memory_write(uint32_t address, char byte);
void device_read(uint32_t address, char& byte);
void device_write(uint32_t address, char byte);

// Device slot address range
#define _DEVICE_ADDRESS_START__  0x40000
#define _DEVICE_ADDRESS_END__    0x80000

#define _DEVICE_INDEX_SZ__    0x10

char deviceIndex[_DEVICE_INDEX_SZ__];

// Initiate the device index
void initiateDeviceIndex(void) {
	
	for (uint8_t i=0; i<_DEVICE_INDEX_SZ__; i++) deviceIndex[i] = 0xff;
	
	char identityByte=0xff;
	uint32_t address=_DEVICE_ADDRESS_START__;
	
	for (uint8_t i=0; i < _DEVICE_INDEX_SZ__; i++) {
		
		device_read(address, identityByte);
		
		if (address >= _DEVICE_ADDRESS_END__) return;
		address += 0x10000;
		
		if (identityByte == 0xff) continue;
		
		deviceIndex[i] = identityByte;
		
	}
	
	return;
}

// Get a device address by its identity byte
uint32_t getDeviceAddress(uint8_t identity_byte) {
	
	uint32_t address=_DEVICE_ADDRESS_START__;
	
	for (uint8_t i=0; i < 0x10; i++) {
		
		char readIdentityByte=0x00;
		
		if (identity_byte == deviceIndex[i]) return address;
		
		address += 0x10000;
		if (address > _DEVICE_ADDRESS_END__) break;
	}
	
	return 0x00000;
}

#define _INTERNAL_MEMORY_SIZE__  1024
uint8_t INTERNAL_MEMORY[_INTERNAL_MEMORY_SIZE__];

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
	_BUS_LOWER_OUT__ = (address & 0xff);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_UPPER_OUT__ = (address >> 16);
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


