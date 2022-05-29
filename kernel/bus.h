//
// IO abstraction for implementation of a bus interface
//    Developed for use on the Retroboard R644


#ifndef ____BUS_INTERFACE__
#define ____BUS_INTERFACE__

#ifdef  __CORE_BUS_

// Address / data bus



#define _BUS_LOWER_PORT_A__
#define _BUS_MIDDLE_PORT_C__
#define _BUS_UPPER_PORT_D__






// Lower address bus
#ifdef _BUS_LOWER_PORT_A__
  #define _BUS_LOWER_DIR__   DDRA
  #define _BUS_LOWER_OUT__   PORTA
  #define _BUS_LOWER_IN__    PINA
#endif
#ifdef _BUS_LOWER_PORT_B__
  #define _BUS_LOWER_DIR__   DDRB
  #define _BUS_LOWER_OUT__   PORTB
  #define _BUS_LOWER_IN__    PINB
#endif
#ifdef _BUS_LOWER_PORT_C__
  #define _BUS_LOWER_DIR__   DDRC
  #define _BUS_LOWER_OUT__   PORTC
  #define _BUS_LOWER_IN__    PINC
#endif
#ifdef _BUS_LOWER_PORT_D__
  #define _BUS_LOWER_DIR__   DDRD
  #define _BUS_LOWER_OUT__   PORTD
  #define _BUS_LOWER_IN__    PIND
#endif

// Middle address bus
#ifdef _BUS_MIDDLE_PORT_A__
  #define _BUS_MIDDLE_DIR__  DDRA
  #define _BUS_MIDDLE_OUT__  PORTA
  #define _BUS_MIDDLE_IN__   PINA
#endif
#ifdef _BUS_MIDDLE_PORT_B__
  #define _BUS_MIDDLE_DIR__  DDRB
  #define _BUS_MIDDLE_OUT__  PORTB
  #define _BUS_MIDDLE_IN__   PINB
#endif
#ifdef _BUS_MIDDLE_PORT_C__
  #define _BUS_MIDDLE_DIR__  DDRC
  #define _BUS_MIDDLE_OUT__  PORTC
  #define _BUS_MIDDLE_IN__   PINC
#endif
#ifdef _BUS_MIDDLE_PORT_D__
  #define _BUS_MIDDLE_DIR__  DDRD
  #define _BUS_MIDDLE_OUT__  PORTD
  #define _BUS_MIDDLE_IN__   PIND
#endif

// Upper address bus
#ifdef _BUS_UPPER_PORT_A__
  #define _BUS_UPPER_DIR__   DDRA
  #define _BUS_UPPER_OUT__   PORTA
  #define _BUS_UPPER_IN__    PINA
#endif
#ifdef _BUS_UPPER_PORT_B__
  #define _BUS_UPPER_DIR__   DDRB
  #define _BUS_UPPER_OUT__   PORTB
  #define _BUS_UPPER_IN__    PINB
#endif
#ifdef _BUS_UPPER_PORT_C__
  #define _BUS_UPPER_DIR__   DDRC
  #define _BUS_UPPER_OUT__   PORTC
  #define _BUS_UPPER_IN__    PINC
#endif
#ifdef _BUS_UPPER_PORT_D__
  #define _BUS_UPPER_DIR__   DDRD
  #define _BUS_UPPER_OUT__   PORTD
  #define _BUS_UPPER_IN__    PIND
#endif

// Control bus

#define _CONTROL_DIR__     DDRB        // Control bus
#define _CONTROL_OUT__     PORTB
#define _CONTROL_IN__      PINB

#define _CONTROL_DATA_DIRECTION__     0x00 // Data direction signal
#define _CONTROL_BUS_ENABLE__         0x01 // Bus enable signal
#define _CONTROL_CLOCK__              0x02 // Peripheral device clock signal
#define _CONTROL_ADDRESS_LATCH__      0x03 // Address latch enable
#define _CONTROL_READ__               0x04 // Read signal
#define _CONTROL_WRITE__              0x05 // Write signal
//#define _CONTROL___                   0x06 // Unused
//#define _CONTROL___                   0x07 // Unused


struct BUSTYPE {
	
	uint8_t  waitstate_read;
	uint8_t  waitstate_write;
	
};

typedef volatile BUSTYPE Bus;


// Initiate and zero the hardware level IO pins
void bus_zero(void);
// Run a write cycle over the bus interface
void bus_write_byte(Bus& bus, uint32_t address, char byte);
// Run a read cycle over the bus interface
void bus_read_byte(Bus& bus, uint32_t address, char& buffer);



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

#endif

