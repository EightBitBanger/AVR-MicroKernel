#include <kernel/kernel.h>

uint32_t fsFileExists(uint8_t* name, uint8_t nameLength) {
    
    struct Bus bus;
    bus.read_waitstate = 5;
    
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
    
    uint8_t workingDirectory[20];
    uint8_t workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
    
    if ((workingDirectoryLength > 0) & (workingDirectory[0] != ' ')) {
        
        uint32_t directoryAddress = fsGetWorkingDirectoryAddress();
        
        // Get directory file size
        union Pointer fileSize;
        for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(&bus, directoryAddress + OFFSET_FILE_SIZE + i, &fileSize.byte_t[i]);
        
        // Get directory size
        union Pointer directorySize;
        for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(&bus, directoryAddress + OFFSET_DIRECTORY_SIZE + i, &directorySize.byte_t[i]);
        
        uint8_t fileBuffer[fileSize.address];
        
        //uint8_t index = fsFileOpen(workingDirectory, workingDirectoryLength-1);
        
        //fsFileReadBin(index, fileBuffer, fileSize.address);
        
        //fsFileClose(index);
        
        /*
        
        // Check the directory for the file
        for (uint32_t d=0; d < directorySize.address; d++) {
            
            // Get the address to the file
            union Pointer fileAddress;
            for (uint8_t i=0; i < 4; i++) 
                fs_read_byte(&bus, fileBuffer[(d * 4) + i], &fileAddress.byte_t[i]);
            
            // Check the filename
            uint8_t filename[10];
            for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
                fs_read_byte(&bus, fileAddress.address + OFFSET_FILE_NAME + i, &filename[i]);
            
            if (StringCompare(filename, 10, name, nameLength) == 1) 
                return fileAddress.address;
            
            continue;
        }
        
        */
        
    } else {
        
        // Root full sweep
        for (uint32_t sector=0; sector < currentCapacity; sector++) {
            
            // Find an active file start byte
            uint8_t headerByte;
            fs_read_byte(&bus, currentDevice + sector * SECTOR_SIZE, &headerByte);
            
            if (headerByte != 0x55) 
                continue;
            
            // Get filename from the device
            uint8_t filenameCheck[FILE_NAME_LENGTH];
            
            for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
                fs_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i, &filenameCheck[i]);
            
            if (StringCompare(name, nameLength, filenameCheck, FILE_NAME_LENGTH) == 1) 
                return currentDevice + (sector * SECTOR_SIZE);
            
            continue;
        }
        
    }
    
    return 0;
}
