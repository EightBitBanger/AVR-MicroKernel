//
// Hardware abstraction layer

#ifndef __BUS_INTERFACE__
#define __BUS_INTERFACE__

#define _BUS_LOWER_DIR__   DDRA   // Lower address/data bus
#define _BUS_LOWER_OUT__   PORTA
#define _BUS_LOWER_IN__    PINA

#define _BUS_MIDDLE_DIR__  DDRC   // Middle address bus
#define _BUS_MIDDLE_OUT__  PORTC
#define _BUS_MIDDLE_IN__   PINC

#define _BUS_UPPER_DIR__   DDRD   // Upper address bus
#define _BUS_UPPER_OUT__   PORTD
#define _BUS_UPPER_IN__    PIND

#define _CONTROL_DIR__     DDRB   // Control bus
#define _CONTROL_OUT__     PORTB
#define _CONTROL_IN__      PINB

#define _CONTROL_DATA_DIRECTION__     0x00 // Data bus direction signal
#define _CONTROL_BUS_ENABLE__         0x01 // Bus enable signal
#define _CONTROL_CLOCK__              0x02 // Peripheral clock signal
#define _CONTROL_ADDRESS_LATCH__      0x03 // Address latch enable
#define _CONTROL_READ__               0x04 // Read signal
#define _CONTROL_WRITE__              0x05 // Write signal
//#define _CONTROL___                   0x06
//#define _CONTROL___                   0x07

#define _INITIAL_SETUP_TIME__  5000

#define _BUS_STATE_CLEAR__   0
#define _BUS_STATE_ENABLE__  1
#define _BUS_STATE_READ__    2
#define _BUS_STATE_WRITE__   3

struct Bus {
	
	uint8_t waitStateRead;
	uint8_t waitStateWrite;
	
	uint8_t state_bus_clear;
	uint8_t state_bus_enable;
	uint8_t state_bus_read;
	uint8_t state_bus_write;
	
	Bus() {
		
		waitStateRead     = 0;
		waitStateWrite    = 0;
		
		state_bus_clear   = 0xff;
		state_bus_enable  = 0xff;
		state_bus_read    = 0xff;
		state_bus_write   = 0xff;
		
	}
	
	Bus(Bus& new_bus) {
		waitStateRead    = new_bus.waitStateRead;
		waitStateWrite   = new_bus.waitStateWrite;
		
		state_bus_clear  = new_bus.state_bus_clear;
		state_bus_enable = new_bus.state_bus_enable;
		state_bus_read   = new_bus.state_bus_read;
		state_bus_write  = new_bus.state_bus_write;
	}
	
	// Initiate the bus interface
	void initiate(void) {
		
		_BUS_LOWER_DIR__=0xff;
		_BUS_LOWER_OUT__=0x00;
		
		_BUS_MIDDLE_DIR__=0xff;
		_BUS_MIDDLE_OUT__=0xff;
		
		_BUS_UPPER_DIR__=0xff;
		_BUS_UPPER_OUT__=0x00;
		
		_CONTROL_DIR__=0xff;
		_CONTROL_OUT__=0xff;
		
		// Allow time for the external circuitry to initialize
		for (uint16_t i=0; i < _INITIAL_SETUP_TIME__; i++) for (uint8_t a=0; a < 255; a++) asm("nop");
		
	}
	
	// Run a read cycle over the bus interface
	void read(uint32_t address, char& buffer) {
		
		// Address the device
		_BUS_LOWER_DIR__ = 0xff;
		
		_CONTROL_OUT__ = state_bus_clear;
		_BUS_LOWER_OUT__ = (address & 0xff);
		_BUS_MIDDLE_OUT__ = (address >> 8);
		_BUS_UPPER_OUT__ = (address >> 16);
		_CONTROL_OUT__ = state_bus_enable;
		
		// Set data direction
		_BUS_LOWER_DIR__ = 0x00; // Set input
		_BUS_LOWER_OUT__ = 0x00; // No internal pull-up resistors
		
		// Begin the read cycle
		_CONTROL_OUT__ = state_bus_read;
		
		// Wait state
		for (uint16_t i=0; i<waitStateRead; i++) asm("nop");
		
		// Read the data byte
		buffer = _BUS_LOWER_IN__;
		
		_CONTROL_OUT__ = state_bus_clear;
		_BUS_UPPER_OUT__ = 0x0f;
		
		return;
	}
	
	// Run a write cycle over the bus interface
	void write(uint32_t address, char byte) {
		
		// Address the device
		_BUS_LOWER_DIR__ = 0xff;
		
		_CONTROL_OUT__ = state_bus_clear;
		_BUS_LOWER_OUT__ = (address & 0xff);
		_BUS_MIDDLE_OUT__ = (address >> 8);
		_BUS_UPPER_OUT__ = (address >> 16);
		_CONTROL_OUT__ = state_bus_enable;
		
		// Cast the data byte
		_BUS_LOWER_OUT__ = byte;
		
		// Begin write cycle
		_CONTROL_OUT__ = state_bus_write;
		
		// Wait state
		for (uint16_t i=0; i<waitStateWrite; i++) asm("nop");
		
		_CONTROL_OUT__ = state_bus_clear;
		_BUS_UPPER_OUT__ = 0x0f;
		
		return;
	}
	
	void enable_state(uint8_t bus_register, uint8_t state) {
		switch (bus_register) {
			case _BUS_STATE_CLEAR__:  state_bus_clear &= ~(1<<state); break;
			case _BUS_STATE_ENABLE__: state_bus_enable &= ~(1<<state); break;
			case _BUS_STATE_READ__:   state_bus_read &= ~(1<<state); break;
			case _BUS_STATE_WRITE__:  state_bus_write &= ~(1<<state); break;
			default: break;
		}
	}
	
	void disable_state(uint8_t bus_register, uint8_t state) {
		switch (bus_register) {
			case _BUS_STATE_CLEAR__:  state_bus_clear |= (1<<state); break;
			case _BUS_STATE_ENABLE__: state_bus_enable |= (1<<state); break;
			case _BUS_STATE_READ__:   state_bus_read |= (1<<state); break;
			case _BUS_STATE_WRITE__:  state_bus_write |= (1<<state); break;
			default: break;
		}
	}
	
};

#endif

