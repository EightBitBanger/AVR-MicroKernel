#include <kernel/kernel.h>

uint32_t fsFileFind(uint32_t index) {
    
    uint32_t currentDevice = fsGetDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    if (currentCapacity == 0) 
        return 0;
    
    uint32_t fileFindIndex = 0;
    
    // Check following sectors allocated to this file
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Find an active file start byte
        if (fsSectorGetByte( sector * SECTOR_SIZE ) != 0x55) 
            continue;
        
        // Check file index
        if (fileFindIndex == index) 
            return currentDevice + (sector * SECTOR_SIZE);
        
        fileFindIndex++;
        
    }
    
    return 0;
}
