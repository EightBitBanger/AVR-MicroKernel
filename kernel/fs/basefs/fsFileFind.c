#include <kernel/fs/fs.h>

uint32_t fileFindIndex = 0;

uint32_t fsFileFindFirst(void) {
    
    if (fsDeviceGetCapacity() == 0)
        return 0;
    
    uint32_t fileAddress = fsFileFind(0);
    
    fileFindIndex = 0;
    
    return fileAddress;
}



uint32_t fsFileFindNext(void) {
    
    fileFindIndex++;
    
    uint32_t fileAddress = fsFileFind( fileFindIndex );
    
    return fileAddress;
    
    return 0;
}


uint32_t fsFileFind(uint32_t index) {
    
    if (fsDeviceGetCapacity() == 0)
        return 0;
    
    uint32_t numberOfSectors = fsDeviceGetCapacity() / FORMAT_SECTOR_SIZE;
    
    uint32_t fileFindIndex = 0;
    
    // Check following sectors allocated to this file
    for (uint32_t sector=0; sector <= numberOfSectors; sector++) {
        
        uint32_t currentSector = sector * FORMAT_SECTOR_SIZE;
        
        // Find an active file start byte
        uint8_t headerByte = 0;
        fs_read_byte(currentSector, &headerByte);
        
        if (headerByte != FORMAT_SECTOR_HEADER) 
            continue;
        
        // Check file index
        if (fileFindIndex == index) 
            return currentSector;
        
        fileFindIndex++;
        
    }
    
    return 0;
}

