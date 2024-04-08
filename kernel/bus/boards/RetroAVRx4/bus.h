#ifdef BOARD_RETRO_AVR_X4_REV1

#define EXTERNAL_MEMORY_BEGIN     0x00000

#define PERIPHERAL_ADDRESS_BEGIN  0x00000
#define PERIPHERAL_STRIDE         0x10000

#define NUMBER_OF_PERIPHERALS  8


#include <kernel/bus/bus.h>

void bus_control_zero(void) {
    
    DDRB = 0b00011001;
    DDRD = 0b11111111;
    
    PORTB = 0b00011001;
    PORTD = 0b11111111;
    
    return;
}

void bus_address_zero(void) {
	
	DDRA = 0xff;
    DDRC = 0xff;
    
    PORTA = 0x00;
    PORTC = 0x00;
    
	return;
}

void bus_read_memory(struct Bus* bus, uint32_t address, uint8_t* buffer) {
	
    DDRA = 0xff; // Output for address
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches / keep the bus disabled
    
    DDRA  = 0x00;  // Set data bus to input
    
    PORTC = (address >> 16); // Set address HIGH
    
    // Latch upper address and set the data bus
    PORTD = 0b11111111; // Open latch high
    PORTD = 0b11011011; // Close latch high / MEMORY bus cycle
    
    PORTB = 0b00000000; // Enable bus
    
    PORTD = 0b10010011; // Memory signal read cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->read_waitstate; wait++) 
        __asm__("nop");
    
    // Read the data byte
    *buffer = PINA; // Read data byte
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    return;
}


void bus_write_memory(struct Bus* bus, uint32_t address, uint8_t byte) {
	
    DDRA = 0xff; // Output for address
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    // Set the lower addresses
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches and keep the bus disabled
    
    PORTC = (address >> 16); // Set address HIGH
    
    // Latch upper address and set data byte
    PORTD = 0b11111111; // Open latch high
    PORTD = 0b11011011; // Close latch high / MEMORY bus cycle
    
    PORTA = byte; // Write data byte
    
    PORTB = 0b00000000; // Enable bus
    
    PORTD = 0b11001011; // Memory signal write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) 
        __asm__("nop");
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    DDRA = 0x00; // Relax the data bus
    
	return;
}

void bus_read_io(struct Bus* bus, uint32_t address, uint8_t* buffer) {
	
    DDRA = 0xff; // Output for address
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches / keep the bus disabled
    
    DDRA  = 0x00;  // Set data bus to input
    
    PORTC = (address >> 16); // Set address HIGH
    
    // Latch upper address and set the data bus
    PORTD = 0b11111111; // Open latch high
    PORTD = 0b11111011; // Close latch high / IO bus cycle
    
    PORTB = 0b00000000; // Enable bus
    
    PORTD = 0b10110011; // IO signal read cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->read_waitstate; wait++) 
        __asm__("nop");
    
    // Read the data byte
    *buffer = PINA; // Read data byte
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    return;
}


void bus_write_io(struct Bus* bus, uint32_t address, uint8_t byte) {
	
    DDRA = 0xff; // Output for address
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    // Set the lower addresses
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches and keep the bus disabled
    
    PORTC = (address >> 16); // Set address HIGH
    
    // Latch upper address and set the data bus
    PORTD = 0b11111111; // Open latch high
    PORTD = 0b11111011; // Close latch high / IO bus cycle
    
    PORTA = byte; // Write data byte
    
    PORTB = 0b00000000; // Enable bus
    
    PORTD = 0b11101011; // IO signal write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) 
        __asm__("nop");
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    DDRA = 0x00; // Relax the data bus
    
	return;
}

void bus_read_byte(struct Bus* bus, uint32_t address, uint8_t* buffer) {
    
    bus_read_io(bus, address, buffer);
    
    return;
}

void bus_write_byte(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    bus_write_io(bus, address, byte);
    
    return;
}

void bus_write_byte_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    DDRA = 0xff; // Output for address
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    // Set the lower addresses
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches and keep the bus disabled
    
    PORTC = (address >> 16); // Set address HIGH
    
    // Latch upper address and set data byte
    PORTD = 0b11111111; // Open latch high
    PORTD = 0b11011011; // Close latch high / MEMORY bus cycle
    
    PORTA = byte; // Write data byte
    
    PORTB = 0b00000000; // Enable bus
    
    PORTD = 0b11001011; // Memory signal write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) {
        
        __asm__("nop");
        
        _delay_ms(1);
        
    }
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    DDRA = 0x00; // Relax the data bus
    
    _delay_ms(5);
    
	return;
}

#endif
