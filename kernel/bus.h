//
// IO abstraction for implementation of a bus interface

#ifndef ____BUS_INTERFACE__
#define ____BUS_INTERFACE__

#ifdef  __CORE_BUS_

// Address / data bus

#define _BUS_LOWER_DIR__   DDRA        // Lower address bus
#define _BUS_LOWER_OUT__   PORTA
#define _BUS_LOWER_IN__    PINA

#define _BUS_MIDDLE_DIR__  DDRC        // Middle address bus
#define _BUS_MIDDLE_OUT__  PORTC
#define _BUS_MIDDLE_IN__   PINC

#define _BUS_UPPER_DIR__   DDRD        // Upper address bus
#define _BUS_UPPER_OUT__   PORTD
#define _BUS_UPPER_IN__    PIND

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


#endif

#endif

