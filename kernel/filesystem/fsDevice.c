#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>

#define FS_DEVICE_TYPE_IO    0
#define FS_DEVICE_TYPE_MEM   1

void(*__fs_read_byte)(struct Bus*, uint32_t, uint8_t*);
void(*__fs_write_byte)(struct Bus*, uint32_t, uint8_t);


uint32_t fs_device_address = 0;

void fsInit(void) {
    
    fsSetDeviceTypeIO();
    
    return;
}

void fs_read_byte(struct Bus* bus, uint32_t address, uint8_t* buffer) {
    
    __fs_read_byte(bus, address, buffer);
    
    return;
}

void fs_write_byte(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    __fs_write_byte(bus, address, byte);
    
    return;
}

void fsSetDeviceTypeIO(void) {
    
    __fs_read_byte  = bus_read_io;
    __fs_write_byte = bus_write_io_eeprom;
    
    return;
}

void fsSetDeviceTypeMEM(void) {
    
    __fs_read_byte  = bus_read_memory;
    __fs_write_byte = bus_write_memory_eeprom;
    
    return;
}

void fsSetCurrentDevice(uint8_t device_index) {
    
    fs_device_address = PERIPHERAL_ADDRESS_BEGIN + (device_index * 0x10000);
    
    return;
}


uint32_t fsGetCurrentDevice(void) {
    
    return fs_device_address;
}


uint8_t fsCheckDeviceReady(void) {
    
    uint8_t deviceHeader[2];
    deviceHeader[0] = fsGetDeviceHeaderByte(1);
    deviceHeader[1] = fsGetDeviceHeaderByte(2);
    
    if ((deviceHeader[0] != 'f') | (deviceHeader[1] != 's')) 
        return 0;
    
    return 1;
}


uint8_t fsGetDeviceHeaderByte(uint32_t address_offset) {
    uint8_t headerByte = 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    fs_read_byte(&bus, fs_device_address + address_offset, &headerByte);
    
    return headerByte;
}


uint32_t fsGetDeviceCapacity(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    uint8_t buffer[SECTOR_SIZE];
    
    // Get header sector
    for (uint8_t i=0; i < SECTOR_SIZE; i++) 
        fs_read_byte(&bus, fsGetCurrentDevice() + i, &buffer[i]);
    
    // Check header byte
    if (buffer[0] != 0x13) 
        return 0;
    
    // Check hardware name
    if ((buffer[1] != 'f') | (buffer[2] != 's')) 
        return 0;
    
    union Pointer sizePointer;
    
    // Get device capacity
    for (uint8_t i=0; i < 4; i++) 
        sizePointer.byte_t[i] = buffer[i + DEVICE_CAPACITY_OFFSET];
    
    return sizePointer.address;
}

