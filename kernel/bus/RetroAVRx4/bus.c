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


#define CACHE_SIZE   32

uint8_t cache[CACHE_SIZE];

uint32_t cache_begin = 0;
uint32_t cache_end   = 0;



void bus_initiate(void) {
    
    bus_control_zero();
    bus_address_zero();
    
    return;
}

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


void bus_raw_read_memory(struct Bus* bus, uint32_t address, uint8_t* buffer) {
	
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


void bus_raw_write_memory(struct Bus* bus, uint32_t address, uint8_t byte) {
	
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

void bus_flush_cache(struct Bus* bus, uint32_t address) {
    
    // Flush the buffer back to main memory
    for (uint32_t i=0; i < CACHE_SIZE; i++) 
        bus_raw_write_memory(bus, cache_begin + i, cache[i]);
    
    // Fetch new data from main memory
    for (uint32_t i=0; i < CACHE_SIZE; i++) 
        bus_raw_read_memory(bus, address + i, &cache[i]);
    
    cache_begin = address;
    cache_end   = address + CACHE_SIZE - 1;
    
    return;
}

void bus_read_memory(struct Bus* bus, uint32_t address, uint8_t* buffer) {
	
	if ((address < cache_begin) | (address > cache_end)) {
        
        bus_flush_cache(bus, address);
        
        *buffer = cache[0];
        
        return;
    }
	
	uint32_t address_offset = address - cache_begin;
	
	*buffer = cache[address_offset];
	
    return;
}


void bus_write_memory(struct Bus* bus, uint32_t address, uint8_t byte) {
	
	if ((address < cache_begin) | (address > cache_end)) {
        
        bus_flush_cache(bus, address);
        
        cache[0] = byte;
        
        return;
    }
	
	uint32_t address_offset = address - cache_begin;
	
	cache[address_offset] = byte;
	
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


void bus_read_byte(struct Bus* bus, uint32_t address, uint8_t* buffer) {
	
	bus_read_memory(bus, address, buffer);
	
	return;
}


void bus_write_byte(struct Bus* bus, uint32_t address, uint8_t byte) {
	
	bus_write_memory(bus, address, byte);
	
	return;
}



void bus_raw_memory_read(struct Bus* bus, uint32_t address, uint8_t* buffer) {
    
	// Output the data bus
    _BUS_LOWER_DIR__ = 0xff;
	
	// Address the device
	_BUS_UPPER_OUT__  = (address >> 16);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_LOWER_OUT__  = (address & 0xff);
	
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_CONTROL_OUT__ = _CONTROL_READ_LATCH__;
	
	// Set data direction
	_BUS_LOWER_DIR__ = 0x00;
	_BUS_LOWER_OUT__ = 0xff; // Internal pull-up resistors
	
	// Wait state
	for (uint16_t i=0; i < bus->read_waitstate; i++) 
        _CONTROL_OUT__ = _CONTROL_READ_CYCLE__;
	
	// Read the data byte
	*buffer = _BUS_LOWER_IN__;
	
	// End cycle
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_BUS_UPPER_OUT__  = 0x00;
	_BUS_MIDDLE_OUT__ = 0x00;
	_BUS_LOWER_OUT__  = 0x00;
	_CONTROL_OUT__ = _CONTROL_CLOSED_LATCH__;
	
}

void bus_raw_memory_write(struct Bus* bus, uint32_t address, uint8_t* buffer) {
    
	// Output the data bus
    _BUS_LOWER_DIR__ = 0xff;
	
	// Address the device
	_BUS_UPPER_OUT__  = (address >> 16);
	_BUS_MIDDLE_OUT__ = (address >> 8);
	_BUS_LOWER_OUT__  = (address & 0xff);
	
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_CONTROL_OUT__ = _CONTROL_WRITE_LATCH__;
	
	// Cast the data byte
	_BUS_LOWER_OUT__ = byte;
	
	// Wait state
	for (uint16_t i=0; i < bus->write_waitstate; i++) 
        _CONTROL_OUT__ = _CONTROL_WRITE_CYCLE__;
	
	// End cycle
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_BUS_UPPER_OUT__  = 0x00;
	_BUS_MIDDLE_OUT__ = 0x00;
	_BUS_LOWER_OUT__  = 0x00;
	_CONTROL_OUT__ = _CONTROL_CLOSED_LATCH__;
	
}


#define CACHE_SIZE   32

uint8_t cache[CACHE_SIZE];
uint32_t cache_ptr = 0xffffffff;


void bus_read_memory(struct Bus* bus, uint32_t address, uint8_t* buffer) {
	
	if (address < cache_ptr) | (address >= (cache_ptr + CACHE_SIZE)) {
        
        // Flush the buffer back to main memory
        for (uint32_t i=0; i < CACHE_SIZE; i++) 
            bus_raw_memory_write(bus, cache_ptr + i, cache[i]);
        
        for (uint32_t i=0; i < CACHE_SIZE; i++) 
            bus_raw_memory_read(bus, address + i, cache[i]);
        
        cache_ptr = address;
    }
	
	uint32_t address_offset = address - cache_ptr;
	
	//buffer = cache[address_offset];
	
	return;
}


void bus_write_memory(struct Bus* bus, uint32_t address, uint8_t byte) {
	
	if (address < cache_ptr) | (address >= (cache_ptr + CACHE_SIZE)) {
        
        // Flush the buffer back to main memory
        for (uint32_t i=0; i < CACHE_SIZE; i++) 
            bus_raw_memory_write(bus, cache_ptr + i, cache[i]);
        
        for (uint32_t i=0; i < CACHE_SIZE; i++) 
            bus_raw_memory_read(bus, address + i, cache[i]);
        
        cache_ptr = address;
    }
	
	uint32_t address_offset = address - cache_ptr;
	
	//cache[address_offset] = buffer;
	
	return;
}


void bus_write_io(struct Bus* bus, uint32_t address, uint8_t byte) {
    bus_write_memory(bus, address, byte);
    return;
}

void bus_read_io(struct Bus* bus, uint32_t address, uint8_t* buffer) {
    bus_read_memory(bus, address, buffer);
    return;
}

void bus_write_io_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    bus_write_byte_eeprom(bus, address, byte);
    return;
}

void bus_write_byte_eeprom(struct Bus* bus, uint32_t address, uint8_t byte) {
    
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
	for (uint16_t i=0; i < bus->write_waitstate; i++) 
        _CONTROL_OUT__ = _CONTROL_WRITE_CYCLE__;
	
	// End cycle
	_CONTROL_OUT__ = _CONTROL_OPEN_LATCH__;
	_BUS_UPPER_OUT__  = 0x00;
	_BUS_MIDDLE_OUT__ = 0x00;
	_BUS_LOWER_OUT__  = 0x00;
	_CONTROL_OUT__ = _CONTROL_CLOSED_LATCH__;
	
	_delay_ms(10);
	
    return;
}


#endif


*/
