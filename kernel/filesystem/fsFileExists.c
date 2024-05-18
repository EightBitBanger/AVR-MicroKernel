#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>


uint32_t fsFileExists(uint8_t* name, uint8_t nameLength) {
    
    struct Bus bus;
    bus.read_waitstate = 4;
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    if (currentCapacity == 0) 
        return 0;
    
    // Verify the capacity
    // Default to minimum size if size unknown
    if ((currentCapacity != CAPACITY_8K) & 
        (currentCapacity != CAPACITY_16K) & 
        (currentCapacity != CAPACITY_32K)) {
        currentCapacity = CAPACITY_8K;
    }
    
    // Check following sectors allocated to this file
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Find an active file start byte
        if (fsGetDeviceHeaderByte( sector * SECTOR_SIZE ) != 0x55) 
            continue;
        
        uint8_t isFileFound = 0;
        
        // Check file name
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
            
            uint8_t nameByte = 0;
            
            bus_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i, &nameByte);
            
            if (name[i] != nameByte) {
                isFileFound = 0;
                break;
            }
            
            isFileFound = 1;
            
            continue;
        }
        
        // Check next file
        if (isFileFound == 0) 
            continue;
        
		return currentDevice + (sector * SECTOR_SIZE);
    }
    
    return 0;
}

