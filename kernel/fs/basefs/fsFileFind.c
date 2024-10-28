#include <kernel/kernel.h>

uint32_t fileFindIndex = 0;

uint32_t fsFileFindFirst(void) {
    
    if (fsGetDeviceCapacity() == 0)
        return 0;
    
    uint32_t fileAddress = fsFileFind(0);
    
    fileFindIndex = 0;
    
    return fileAddress;
}



uint32_t fsFileFindNext(void) {
    
    fileFindIndex++;
    
    uint32_t fileAddress = fsFileFind( fileFindIndex );
    
    return fileAddress;
    
    /*
    
    uint32_t numberOfSectors = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    // Check following sectors allocated to this file
    for (fileFindAddress++; fileFindAddress <= numberOfSectors; fileFindAddress++) {
        
        uint32_t currentSector = fileFindAddress * SECTOR_SIZE;
        
        // Find an active file start byte
        uint8_t headerByte = 0;
        fs_read_byte(currentSector, &headerByte);
        
        if (headerByte != SECTOR_HEADER) 
            continue;
        
        // Return file index
        return currentSector;
    }
    */
    return 0;
}


uint32_t fsFileFind(uint32_t index) {
    
    if (fsGetDeviceCapacity() == 0)
        return 0;
    
    uint32_t numberOfSectors = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    uint32_t fileFindIndex = 0;
    
    // Check following sectors allocated to this file
    for (uint32_t sector=0; sector <= numberOfSectors; sector++) {
        
        uint32_t currentSector = sector * SECTOR_SIZE;
        
        // Find an active file start byte
        uint8_t headerByte = 0;
        fs_read_byte(currentSector, &headerByte);
        
        if (headerByte != SECTOR_HEADER) 
            continue;
        
        // Check file index
        if (fileFindIndex == index) 
            return currentSector;
        
        fileFindIndex++;
        
    }
    
    return 0;
}

