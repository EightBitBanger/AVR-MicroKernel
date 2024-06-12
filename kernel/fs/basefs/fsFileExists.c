#include <kernel/kernel.h>


uint32_t fsFileExists(uint8_t* name, uint8_t nameLength) {
    
    uint32_t currentDevice = fsGetDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    // Root full sweep
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Find an active file start byte
        uint8_t headerByte;
        fs_read_byte(currentDevice + (sector * SECTOR_SIZE), &headerByte);
        
        if (headerByte != 0x55) 
            continue;
        
        // Get filename from the device
        uint8_t filenameCheck[FILE_NAME_LENGTH];
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            fs_read_byte(currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i, &filenameCheck[i]);
        
        if (StringCompare(name, nameLength, filenameCheck, FILE_NAME_LENGTH) == 1) 
            return currentDevice + (sector * SECTOR_SIZE);
        
        continue;
    }
    
    return 0;
}
