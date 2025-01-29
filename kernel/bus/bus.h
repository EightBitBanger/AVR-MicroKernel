#ifndef __BUS_INTERFACE_
#define __BUS_INTERFACE_


struct Bus {
    
    /// Linked list of devices on this bus
    uint8_t numberOfDevices;
    struct Device* devicePtr;
    
    /// Number of wait cycles to perform during the read operation
    uint16_t read_waitstate;
    /// Number of wait cycles to perform during the write operation
    uint16_t write_waitstate;
    
    /// Physical interface type
    uint8_t bus_type;
    
};


/// Initiate the bus for interfacing.
void bus_initiate(void);
/// Zero the control logic IO port.
void bus_control_zero(void);
/// Zero the address bus IO ports.
void bus_address_zero(void);


/// Read a byte from an address using the given bus.
void bus_read_byte(struct Bus* bus, uint32_t address, uint8_t* buffer);
/// Write a byte to an address using the given bus.
void bus_write_byte(struct Bus* bus, uint32_t address, uint8_t byte);

// IO memory space

/// Read a byte from IO address space.
void bus_read_io(struct Bus* bus, uint32_t address, uint8_t* buffer);
/// Write a byte to IO address space.
void bus_write_io(struct Bus* bus, uint32_t address, uint8_t byte);

// RAM memory space using caching

/// Read a byte from memory address space.
void bus_read_memory(struct Bus* bus, uint32_t address, uint8_t* buffer);
/// Write a byte to memory address space.
void bus_write_memory(struct Bus* bus, uint32_t address, uint8_t byte);

// RAM memory space direct access

/// Write a byte directly to memory.
void bus_raw_write_memory(struct Bus* bus, uint32_t address, uint8_t byte);
/// Read a byte directly from memory.
void bus_raw_read_memory(struct Bus* bus, uint32_t address, uint8_t* buffer);

// EEPROM access

/// Write a byte to an address using the given bus.
/// 5 milliseconds wait state for EEPROM writes
void bus_write_byte_eeprom(struct Bus* bus, uint32_t address, uint8_t byte);

void bus_write_memory_eeprom(struct Bus* bus, uint32_t address, uint8_t byte);

void bus_write_io_eeprom(struct Bus* bus, uint32_t address, uint8_t byte);

#endif
