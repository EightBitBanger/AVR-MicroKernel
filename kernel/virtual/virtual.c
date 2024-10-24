#include <avr/io.h>

#include <kernel/virtual/virtual.h>


void VirtualWrite(uint32_t address, uint8_t* byte, uint32_t size) {
    uint8_t deviceLetter = fsGetDeviceRoot();
    fsSetDeviceLetter('x');
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = address + SECTOR_SIZE + sector + 1;
        
        fs_write_byte(positionOffset, byte[i]);
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    fsSetDeviceLetter(deviceLetter);
    return;
}


void VirtualRead(uint32_t address, uint8_t* byte, uint32_t size) {
    uint8_t deviceLetter = fsGetDeviceRoot();
    fsSetDeviceLetter('x');
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = address + SECTOR_SIZE + sector + 1;
        
        fs_read_byte(positionOffset, &byte[i]);
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    fsSetDeviceLetter(deviceLetter);
    return;
}

