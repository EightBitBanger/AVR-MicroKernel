#include "config.h"
#ifdef  __CORE_BUS_

#define _CLOCK_SPEED_MHZ__  24.0
#define F_CPU  _CLOCK_SPEED_MHZ__ * 1000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "bus.h"

// Initiate and zero the hardware level IO
void bus_zero(void) {
	
	// Address bus
	_BUS_LOWER_DIR__=0xff;
	_BUS_LOWER_OUT__=0x00;
	
	_BUS_MIDDLE_DIR__=0xff;
	_BUS_MIDDLE_OUT__=0xff;
	
	_BUS_UPPER_DIR__=0xff;
	_BUS_UPPER_OUT__=0x00;
	
	// Control bus
	_CONTROL_DIR__=0xff;
	_CONTROL_OUT__=0xff;
	
}

// Run a read cycle over the bus interface
void bus_read_byte(Bus& bus, uint32_t address, char& buffer) {
	
	// Address the device
	_BUS_LOWER_DIR__ = 0xff;
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_LOWER_OUT__ = (address & 0xff);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_UPPER_OUT__ = (address >> 16);
	_CONTROL_OUT__ = 0b00110100;
	
	// Set data direction
	_BUS_LOWER_DIR__ = 0x00; // Set input
	_BUS_LOWER_OUT__ = 0x00; // No internal pull-up resistors
	
	_CONTROL_OUT__ = 0b00100100; // Begin read cycle
	
	// Wait state
	for (uint16_t i=0; i<bus.waitstate_read; i++) 
		asm("nop");
	
	// Read the data byte
	buffer = _BUS_LOWER_IN__;
	
	_CONTROL_OUT__ = 0b00111111; // End read cycle
	_BUS_UPPER_OUT__ = 0x0f;
	
	return;
}

// Run a write cycle over the bus interface
void bus_write_byte(Bus& bus, uint32_t address, char byte) {
	
	// Address the device
	_BUS_LOWER_DIR__ = 0xff;
	
	_CONTROL_OUT__ = 0b00111111;
	_BUS_LOWER_OUT__ = (address & 0xff);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_UPPER_OUT__ = (address >> 16);
	_CONTROL_OUT__ = 0b00110101;
	
	// Cast the data byte
	_BUS_LOWER_OUT__ = byte;
	
	_CONTROL_OUT__ = 0b00010101; // Begin write cycle
	
	// Wait state
	for (uint16_t i=0; i<bus.waitstate_write; i++) 
		asm("nop");
	
	_CONTROL_OUT__ = 0b00111111; // End write cycle
	_BUS_UPPER_OUT__ = 0x0f;
	
	return;
}


#endif

