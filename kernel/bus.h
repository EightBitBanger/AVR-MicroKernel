//
// Hardware bus abstraction

// Lower address bus
#define _BUS_LOWER_DIR__   DDRA
#define _BUS_LOWER_OUT__   PORTA
#define _BUS_LOWER_IN__    PINA

// Middle address bus
#define _BUS_MIDDLE_DIR__  DDRC
#define _BUS_MIDDLE_OUT__  PORTC
#define _BUS_MIDDLE_IN__   PINC

// Upper address bus
#define _BUS_UPPER_DIR__   DDRD
#define _BUS_UPPER_OUT__   PORTD
#define _BUS_UPPER_IN__    PIND

// Control bus
#define _CONTROL_DIR__     DDRB
#define _CONTROL_OUT__     PORTB
#define _CONTROL_IN__      PINB

//
// Control bits
#define _CONTROL_DATA_DIRECTION__     0x00 // Data direction signal
#define _CONTROL_BUS_ENABLE__         0x01 // Bus enable signal
#define _CONTROL_CLOCK__              0x02 // Peripheral device clock signal
#define _CONTROL_ADDRESS_LATCH__      0x03 // Address latch enable
#define _CONTROL_READ__               0x04 // Read signal
#define _CONTROL_WRITE__              0x05 // Write signal
//#define _CONTROL__                  0x06
//#define _CONTROL__                  0x07

#define _INITIAL_SETUP_TIME__  300  // Ms



struct BusInterface {
	
	// Rate of data transfer
	uint8_t  waitstate_read;
	uint8_t  waitstate_write;
	
	BusInterface() {
		
		waitstate_read  = 0;
		waitstate_write = 0;
		
	}
	
	// Initiate hardware level IO for implementation of the bus interface
	void initiate() {
		
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
		
		// Allow time for the external circuitry to stabilize
		for (uint16_t i=0; i < _INITIAL_SETUP_TIME__; i++) for (uint8_t a=0; a < 255; a++) asm("nop");
		
	}
	
	// Run a read cycle over the bus interface
	void read(uint32_t address, char& buffer) {
		
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
		
		// Wait state
		for (uint16_t i=0; i<waitstate_read; i++) asm("nop");
		
		// Read the data byte
		buffer = _BUS_LOWER_IN__;
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_UPPER_OUT__ = 0x0f;
		
		return;
	}
	// Run a write cycle over the bus interface
	void write(uint32_t address, char byte) {
		
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
		
		// Wait state
		for (uint16_t i=0; i<waitstate_write; i++) asm("nop");
		
		_CONTROL_OUT__ = 0b00111111;
		_BUS_UPPER_OUT__ = 0x0f;
		
		return;
	}
	
};


