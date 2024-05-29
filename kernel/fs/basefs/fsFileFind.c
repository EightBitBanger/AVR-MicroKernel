#include <kernel/kernel.h>

uint32_t fsFileFind(uint32_t index) {
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    if (currentCapacity == 0) 
        return 0;
    
    uint32_t fileFindIndex = 0;
    
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
        
        // Check file index
        if (fileFindIndex == index) 
            return currentDevice + (sector * SECTOR_SIZE);
        
        fileFindIndex++;
        
    }
    
    return 0;
}
