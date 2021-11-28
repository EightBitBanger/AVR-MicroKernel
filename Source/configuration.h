//
// Hardware configuration

//#define _CLOCK_SPEED_MHZ__  16.0
//#define _CLOCK_SPEED_MHZ__  20.0
#define _CLOCK_SPEED_MHZ__  24.0

// Time to allow the system to stabilize
#define  _INITIAL_SETUP_TIME__        1000  // ms

// Use external extended memory
#define _USE_EXTENDED_MEMORY__



//
// Extended memory direct interface
#ifdef _USE_EXTENDED_MEMORY__

//
// Memory wait states

//#define _MEMORY_READ_WAIT_STATE_1__      // 1 nop
#define _MEMORY_READ_WAIT_STATE_2__      // 2 nops
//#define _MEMORY_READ_WAIT_STATE_3__      // 3 nops

//#define _MEMORY_WRITE_WAIT_STATE_1__     // 1 nop
//#define _MEMORY_WRITE_WAIT_STATE_2__     // 2 nops
//#define _MEMORY_WRITE_WAIT_STATE_3__     // 3 nops

//
// Device wait states

//#define _DEVICE_READ_WAIT_STATE_1__
#define _DEVICE_READ_WAIT_STATE_2__
//#define _DEVICE_READ_WAIT_STATE_3__

//#define _DEVICE_WRITE_WAIT_STATE_1__
#define _DEVICE_WRITE_WAIT_STATE_2__
//#define _DEVICE_WRITE_WAIT_STATE_3__





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

// Initiate board level IO for implementation
// of the extended memory interface.
#define initiate_board  _BUS_LOWER_DIR__=0xff; _BUS_LOWER_OUT__=0x00; _BUS_MIDDLE_DIR__=0xff;_BUS_MIDDLE_OUT__=0xff; _BUS_UPPER_DIR__=0xff; _BUS_UPPER_OUT__=0x00; _CONTROL_DIR__=0xff;_CONTROL_OUT__=0xff; _delay_ms(_INITIAL_SETUP_TIME__)

#endif


// JTAG
#define  _DISABLE_JTAG__   MCUCR |=(1<<JTD); MCUCR |=(1<<JTD)


#define nop  asm("nop")

// Calculate the software delay
#define F_CPU  _CLOCK_SPEED_MHZ__ * 1000000UL // Convert to hertz




