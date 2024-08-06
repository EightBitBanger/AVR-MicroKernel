#ifndef __BUS_INTERFACE_
#define __BUS_INTERFACE_

/// Zero the control logic IO port.
void bus_control_zero(void);

/// Zero the address bus IO ports.
void bus_address_zero(void);

/// General purpose bus interface
struct Bus {
    
    uint16_t read_waitstate;
    
    uint16_t write_waitstate;
    
    uint8_t bus_type;
    
};

/// Read a byte from an address using the given bus.
void bus_read_byte(struct Bus* bus, uint32_t address, uint8_t* buffer);

/// Write a byte to an address using the given bus.
void bus_write_byte(struct Bus* bus, uint32_t address, uint8_t byte);


/// Read a byte from IO address space.
void bus_read_io(struct Bus* bus, uint32_t address, uint8_t* buffer);

/// Write a byte to IO address space.
void bus_write_io(struct Bus* bus, uint32_t address, uint8_t byte);


/// Read a byte from memory address space.
void bus_read_memory(struct Bus* bus, uint32_t address, uint8_t* buffer);

/// Write a byte to memory address space.
void bus_write_memory(struct Bus* bus, uint32_t address, uint8_t byte);


/// Write a byte to an address using the given bus.
/// 5 milliseconds wait state for EEPROM writes
void bus_write_byte_eeprom(struct Bus* bus, uint32_t address, uint8_t byte);

void bus_write_memory_eeprom(struct Bus* bus, uint32_t address, uint8_t byte);

void bus_write_io_eeprom(struct Bus* bus, uint32_t address, uint8_t byte);

#endif
