#include <kernel/bus.h>


void bus_control_zero(void) {
    _CONTROL_DIR__=0xff;
	_CONTROL_OUT__=0xff;
	return;
}

void bus_address_zero(void) {
	_BUS_LOWER_DIR__=0xff;
	_BUS_LOWER_OUT__=0xff;
	
	_BUS_MIDDLE_DIR__=0xff;
	_BUS_MIDDLE_OUT__=0xff;
	
	_BUS_UPPER_DIR__=0x0f;
	_BUS_UPPER_OUT__=0xff;
	return;
}

void bus_read_byte(struct Bus* bus, uint32_t address, char* buffer) {
	
	// Address the device
	_BUS_LOWER_DIR__ = 0xff;
	
	_BUS_UPPER_OUT__  = (address >> 16);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_LOWER_OUT__  = (address & 0xff);
	
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_CONTROL_OUT__ = _CONTROL_READ_LATCH__;
	
	// Set data direction
	_BUS_LOWER_DIR__ = 0x00;
	_BUS_LOWER_OUT__ = 0xff; // Internal pull-up resistors
	
	// Wait state
	for (uint8_t i=0; i < bus->read_waitstate; i++) 
        _CONTROL_OUT__ = _CONTROL_READ_CYCLE__;
	
	// Read the data byte
	*buffer = _BUS_LOWER_IN__;
	
	// End cycle
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_BUS_UPPER_OUT__  = 0x00;
	_BUS_MIDDLE_OUT__ = 0x00;
	_BUS_LOWER_OUT__  = 0x00;
	_CONTROL_OUT__ = _CONTROL_CLOSED_LATCH__;
	
	return;
}


void bus_write_byte(struct Bus* bus, uint32_t address, char byte) {
	
	// Address the device
	_BUS_LOWER_DIR__ = 0xff;
	
	_BUS_UPPER_OUT__  = (address >> 16);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_LOWER_OUT__  = (address & 0xff);
	
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_CONTROL_OUT__ = _CONTROL_WRITE_LATCH__;
	
	// Cast the data byte
	_BUS_LOWER_OUT__ = byte;
	
	// Wait state
	for (uint8_t i=0; i < bus->write_waitstate; i++) 
        _CONTROL_OUT__ = _CONTROL_WRITE_CYCLE__;
	
	// End cycle
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_BUS_UPPER_OUT__  = 0x00;
	_BUS_MIDDLE_OUT__ = 0x00;
	_BUS_LOWER_OUT__  = 0x00;
	_CONTROL_OUT__ = _CONTROL_CLOSED_LATCH__;
	
	return;
}


