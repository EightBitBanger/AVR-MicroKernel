#ifndef __BUS_INTERFACE_
#define __BUS_INTERFACE_

struct Bus {
    
    uint16_t read_waitstate;
    
    uint16_t write_waitstate;
    
};


#include <kernel/bus/boards/Retroboard_REV2/bus.h>

/// Zero the control logic IO port.
void bus_control_zero(void);

/// Zero the address bus IO ports.
void bus_address_zero(void);


/// Read a byte from an address using the given bus.
void bus_read_byte(struct Bus* bus, uint32_t address, uint8_t* buffer);

/// Write a byte to an address using the given bus.
void bus_write_byte(struct Bus* bus, uint32_t address, uint8_t byte);

/// Write a byte to an address using the given bus.
/// 5 milliseconds wait state for EEPROM writes
void bus_write_byte_eeprom(uint32_t address, uint8_t byte);

#endif
