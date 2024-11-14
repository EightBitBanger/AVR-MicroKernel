#include <avr/io.h>

#include <kernel/virtual/virtual.h>

#define __KERNEL_VIRTUAL_ACCESS_BEGIN__    0x00000000
#define __KERNEL_VIRTUAL_ACCESS_END__      0xffffffff

#define __SERVICE_VIRTUAL_ACCESS_BEGIN__   0x00000000
#define __SERVICE_VIRTUAL_ACCESS_END__     0xffffffff

#define __USER_VIRTUAL_ACCESS_BEGIN__      0x00000000
#define __USER_VIRTUAL_ACCESS_END__        0xffffffff


uint32_t VirtualAddressBegin = __KERNEL_VIRTUAL_ACCESS_BEGIN__;
uint32_t VirtualAddressEnd   = __KERNEL_VIRTUAL_ACCESS_END__;

int8_t VirtualAccessGetMode(void) {
    
    if (VirtualAddressBegin == __KERNEL_VIRTUAL_ACCESS_BEGIN__) 
        return VIRTUAL_ACCESS_MODE_KERNEL;
    if (VirtualAddressBegin == __SERVICE_VIRTUAL_ACCESS_BEGIN__) 
        return VIRTUAL_ACCESS_MODE_SERVICE;
    if (VirtualAddressBegin == __USER_VIRTUAL_ACCESS_BEGIN__) 
        return VIRTUAL_ACCESS_MODE_USER;
    
    return -1;
}

void VirtualAccessSetMode(uint8_t mode) {
    
    switch (mode) {
        
        case VIRTUAL_ACCESS_MODE_KERNEL: {
            VirtualAddressBegin = __KERNEL_VIRTUAL_ACCESS_BEGIN__;
            VirtualAddressEnd   = __KERNEL_VIRTUAL_ACCESS_END__;
            break;
        }
        
        case VIRTUAL_ACCESS_MODE_SERVICE: {
            VirtualAddressBegin = __SERVICE_VIRTUAL_ACCESS_BEGIN__;
            VirtualAddressEnd   = __SERVICE_VIRTUAL_ACCESS_END__;
            break;
        }
        
        default:
        
        case VIRTUAL_ACCESS_MODE_USER: {
            VirtualAddressBegin = __USER_VIRTUAL_ACCESS_BEGIN__;
            VirtualAddressEnd   = __USER_VIRTUAL_ACCESS_END__;
            break;
        }
        
    }
    
    return;
}


void VirtualWrite(uint32_t address, uint8_t* byte, uint32_t size) {
    
    VirtualBegin();
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (FORMAT_SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t offset = address + FORMAT_SECTOR_SIZE + sector + 1;
        
        // Check segmentation fault
        if ((offset < VirtualAddressBegin) | 
            (offset >= VirtualAddressEnd)) {
            
            kThrow(KERNEL_HALT_SEGMENTATION, offset);
        }
        
        fs_write_byte(offset, byte[i]);
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    VirtualEnd();
    
    return;
}


void VirtualRead(uint32_t address, uint8_t* byte, uint32_t size) {
    
    VirtualBegin();
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (FORMAT_SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t offset = address + FORMAT_SECTOR_SIZE + sector + 1;
        
        // Check segmentation fault
        if ((offset < VirtualAddressBegin) | 
            (offset >= VirtualAddressEnd)) {
            
            kThrow(KERNEL_HALT_SEGMENTATION, offset);
        }
        
        fs_read_byte(offset, &byte[i]);
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    VirtualEnd();
    
    return;
}


uint8_t currentDevice = ' ';

void VirtualBegin(void) {
    
    if (currentDevice != ' ') {
        kThrow(KERNEL_HALT_SEGMENTATION, 0);
        return;
    }
    
    currentDevice = fsDeviceGetRoot();
    fsDeviceSetLetter('x');
    
    return;
}

void VirtualEnd(void) {
    
    if (currentDevice == ' ') {
        kThrow(KERNEL_HALT_SEGMENTATION, 0);
        return;
    }
    
    fsDeviceSetLetter(currentDevice);
    
    currentDevice = ' ';
    
    return;
}

