#include <kernel/kernel.h>


uint32_t fsFileExists(uint8_t* name, uint8_t nameLength) {
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    // Root full sweep
    for (uint32_t sector=0; sector <= currentCapacity; sector++) {
        
        uint32_t currentSector = sector * SECTOR_SIZE;
        
        // Find an active file start byte
        uint8_t headerByte;
        fs_read_byte(currentSector, &headerByte);
        
        if (headerByte != SECTOR_HEADER) 
            continue;
        
        // Check file belongs to root
        uint8_t flag = 0;
        fs_read_byte(currentSector + DIRECTORY_OFFSET_FLAG, &flag);
        
        if (flag != 0) 
            continue;
        
        // Get filename from the device
        uint8_t filenameCheck[FILE_NAME_LENGTH];
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            fs_read_byte(currentSector + FILE_OFFSET_NAME + i, &filenameCheck[i]);
        
        if (StringCompare(name, nameLength, filenameCheck, FILE_NAME_LENGTH) == 1) 
            return currentSector;
        
        continue;
    }
    
    return 0;
}
