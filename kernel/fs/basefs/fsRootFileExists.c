#include <kernel/kernel.h>


uint32_t fsRootFileExists(uint8_t* name, uint8_t nameLength) {
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    // Root full sweep
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Find an active file start byte
        uint8_t headerByte;
        headerByte = fsSectorGetByte(sector * SECTOR_SIZE);
        
        if (headerByte != 0x55) 
            continue;
        
        // Check file is on the root
        if (fsSectorGetByte((sector * SECTOR_SIZE) + OFFSET_DIRECTORY_FLAG) != 0) 
            continue;
        
        // Get filename from the device
        uint8_t filenameCheck[FILE_NAME_LENGTH];
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            filenameCheck[i] = fsSectorGetByte((sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i);
        
        if (StringCompare(filenameCheck, FILE_NAME_LENGTH, name, nameLength) == 1) 
            return fsGetDevice() + (sector * SECTOR_SIZE);
        
        continue;
    }
    
    return 0;
}
