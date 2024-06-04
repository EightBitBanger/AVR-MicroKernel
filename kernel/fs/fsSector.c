#include <kernel/kernel.h>

extern uint32_t fs_device_address;
extern struct Bus fs_bus;


uint8_t fsSectorGetByte(uint32_t address) {
    
    uint8_t byte;
    
    fs_read_byte(&fs_bus, fs_device_address + address, &byte);
    
    return byte;
}

void fsSectorSetByte(uint32_t address, uint8_t byte) {
    
    fs_write_byte(&fs_bus, fs_device_address + address, byte);
    
    return;
}

