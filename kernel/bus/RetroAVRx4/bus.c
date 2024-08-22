#include <avr/io.h>

#include <kernel/delay.h>
#include <kernel/kernel.h>

#include <kernel/bus/bus.h>

#ifdef BOARD_RETRO_AVR_X4_REV1

// 0b10111111 DT/R
// 0b11011111 IO/M
// 0b11101111 Write
// 0b11110111 Read


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
    
    // Set the lower addresses
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches
    
    // Latch upper address
    PORTC = (address >> 16); // Set address HIGH
    PORTD = 0b10011111; // Open latch high
    PORTD = 0b10011011; // Close latch high
    
    // Set data bus to input for read
    DDRA  = 0x00;
    
    PORTB = 0b00000000; // Enable bus
    PORTD = 0b10010011; // Memory read cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->read_waitstate; wait++) 
        __asm__("nop");
    
    // Read the data byte
    *buffer = PINA; // Read data byte
    
    PORTD = 0b10011011; // Finalize the read cycle
    
    PORTA = 0xff;
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    return;
}


void bus_write_memory(struct Bus* bus, uint32_t address, uint8_t byte) {
	
    DDRA = 0xff; // Output for address
    
    // Set the lower addresses
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches
    
    // Latch upper address
    PORTC = (address >> 16); // Set address HIGH
    PORTD = 0b11011111; // Open latch high
    PORTD = 0b11011011; // Close latch high
    
    // Write data byte for write
    PORTA = byte; // Write data byte
    
    PORTB = 0b00000000; // Enable bus
    PORTD = 0b11001011; // Memory write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) 
        __asm__("nop");
    
    PORTD = 0b11011011; // Finalize the write cycle
    
    PORTA = 0xff;
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
	return;
}







void bus_read_io(struct Bus* bus, uint32_t address, uint8_t* buffer) {
	
    DDRA = 0xff; // Output for address
    
    // Set the lower addresses
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches
    
    // Latch upper address
    PORTC = (address >> 16); // Set address HIGH
    PORTD = 0b10111111; // Open latch high
    PORTD = 0b10111011; // Close latch high
    
    // Set data bus to input for read
    DDRA  = 0x00;
    
    PORTB = 0b00000000; // Enable bus
    PORTD = 0b10110011; // IO read cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->read_waitstate; wait++) 
        __asm__("nop");
     
    // Read the data byte
    *buffer = PINA; // Read data byte
    
    PORTD = 0b10111011; // Finalize the read cycle
    
    PORTA = 0xff;
    
    PORTD = 0b11111111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    return;
}


void bus_write_io(struct Bus* bus, uint32_t address, uint8_t byte) {
	
    DDRA = 0xff; // Output for address
    
    // Set the lower addresses
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    PORTB = 0b00011001; // Open latch LOW and MID
    PORTB = 0b00010001; // Close latches
    
    // Latch upper address
    PORTC = (address >> 16); // Set address HIGH
    PORTD = 0b11111111; // Open latch high
    PORTD = 0b11111011; // Close latch high
    
    // Write data byte for write
    PORTA = byte; // Write data byte
    
    PORTB = 0b00000000; // Enable bus
    PORTD = 0b11101011; // IO write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) 
        __asm__("nop");
    
    
    PORTD = 0b11111011; // Finalize the write cycle
    
    PORTA = 0xff;
    
    PORTD = 0b11111111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
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

void bus_write_io_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    bus_write_io(bus, address, byte);
    
    _delay_ms(10);
    
    return;
}

void bus_write_memory_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    bus_write_memory(bus, address, byte);
    
    _delay_ms(10);
    
    return;
}

void bus_write_byte_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    bus_write_io_eeprom(bus, address, byte);
    
    _delay_ms(10);
    
    return;
}

#endif



































/*













#ifdef BOARD_RETRO_AVR_X4_REV1

// 0b10111111 DT/R
// 0b11011111 IO/M
// 0b11101111 Write
// 0b11110111 Read

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
	
    // Set the lower addresses
    DDRA = 0xff; // Output for address
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    PORTB = 0b00010001; // Close latches
    
    
    
    DDRA  = 0x00;  // Set data bus to input
    
    PORTB = 0b00000000; // Enable bus
    
    // Latch upper address and set the data bus
    PORTD = 0b10011011; // Close latch high
    
    PORTC = (address >> 16); // Set address HIGH
    
    PORTD = 0b10010011; // Memory signal read cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->read_waitstate; wait++) 
        __asm__("nop");
    
    // Read the data byte
    *buffer = PINA; // Read data byte
    
    PORTD = 0b10011011; // Finish read cycle
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    return;
}


void bus_write_memory(struct Bus* bus, uint32_t address, uint8_t byte) {
	
    // Set the lower addresses
    DDRA = 0xff; // Output for address
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    PORTB = 0b00010001; // Close latches
    
    
    
    // Latch upper address and set data byte
    PORTD = 0b11011111; // Open latch high
    
    PORTC = (address >> 16); // Set address HIGH
    
    PORTD = 0b11011011; // Close latch high
    
    
    PORTB = 0b00000000; // Enable bus
    
    
    PORTA = byte; // Write data byte
    
    PORTD = 0b11001011; // Memory signal write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) 
        __asm__("nop");
    
    PORTD = 0b11011011; // Finish the write cycle
    
    PORTD = 0b11111111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    DDRA = 0x00; // Relax the data bus
    
    return;
}







void bus_read_io(struct Bus* bus, uint32_t address, uint8_t* buffer) {
	
    // Set the lower addresses
    DDRA = 0xff; // Output for address
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    PORTB = 0b00010001; // Close latches
    
    
    // Latch upper address and set the data bus
    PORTD = 0b10111111; // Open latch high
    
    PORTC = (address >> 16); // Set address HIGH
    
    PORTD = 0b10111011; // Close latch high
    
    
    DDRA  = 0x00;  // Set data bus to input
    
    
    PORTB = 0b00000000; // Enable bus
    
    
    PORTD = 0b10110011; // IO signal read cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->read_waitstate; wait++) 
        __asm__("nop");
    
    // Read the data byte
    *buffer = PINA; // Read data byte
    
    PORTD = 0b10111011; // Finalize the read cycle
    
    PORTD = 0b11111111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    return;
}


void bus_write_io(struct Bus* bus, uint32_t address, uint8_t byte) {
	
    // Set the lower addresses
    DDRA = 0xff; // Output for address
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    PORTB = 0b00010001; // Close latches and keep the bus disabled
    
    
    // Latch upper address and set the data bus
    PORTD = 0b11111111; // Open latch high
    
    PORTC = (address >> 16); // Set address HIGH
    
    PORTD = 0b11111011; // Close latch high
    
    PORTB = 0b00000000; // Enable bus
    
    PORTA = byte; // Write data byte
    
    
    PORTD = 0b11101011; // IO signal write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) 
        __asm__("nop");
    
    PORTD = 0b11111011; // Finalize the write cycle
    
    PORTD = 0b11111111; // Release the bus
    
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

void bus_write_io_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    DDRA = 0xff; // Output for address
    
    // Set the lower addresses
    PORTB = 0b00011001; // Open latch LOW and MID
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    PORTB = 0b00010001; // Close latches and keep the bus disabled
    
    // Latch upper address and set data byte
    PORTD = 0b11111111; // Open latch high
    
    PORTC = (address >> 16); // Set address HIGH
    
    PORTD = 0b11111011; // Close latch high
    
    PORTA = byte; // Write data byte
    
    PORTB = 0b00000000; // Enable bus
    
    PORTD = 0b11101011; // IO signal write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) 
        __asm__("nop");
    
    PORTD = 0b11111111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    DDRA = 0x00; // Relax the data bus
    
    _delay_ms(10);
    
	return;
}

void bus_write_memory_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    DDRA = 0xff; // Output for address
    
    // Set the lower addresses
    PORTB = 0b00011001; // Open latch LOW and MID
    
    PORTA = (address & 0xff); // Set address LOW
    PORTC = (address >> 8);   // Set address MID
    
    PORTB = 0b00010001; // Close latches and keep the bus disabled
    
    // Latch upper address and set data byte
    PORTD = 0b11011111; // Open latch high
    
    PORTC = (address >> 16); // Set address HIGH
    
    PORTD = 0b11011011; // Close latch high
    
    PORTA = byte; // Write data byte
    
    PORTB = 0b00000000; // Enable bus
    
    PORTD = 0b11001011; // Memory signal write cycle
    
    // Waitstate
    for (uint16_t wait=0; wait < bus->write_waitstate; wait++) 
        __asm__("nop");
    
    PORTD = 0b11011111; // Release the bus
    
    PORTB = 0b00011001; // Open latch LOW and MID
    
    DDRA = 0x00; // Relax the data bus
    
    _delay_ms(10);
    
	return;
}

void bus_write_byte_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    bus_write_io_eeprom(bus, address, byte);
    
    return;
}

#endif
*/
