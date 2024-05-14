#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>

uint32_t fs_device_address = 0;


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
    
    bus_read_byte(&bus, fs_device_address + address_offset, &headerByte);
    
    return headerByte;
}


uint32_t fsGetDeviceCapacity(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    bus.write_waitstate = 5;
    
    uint8_t buffer[SECTOR_SIZE];
    
    // Get header sector
    for (uint8_t i=0; i < SECTOR_SIZE; i++) 
        bus_read_byte(&bus, fsGetCurrentDevice() + i, &buffer[i]);
    
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

