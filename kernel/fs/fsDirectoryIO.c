#include <kernel/kernel.h>

uint8_t fsDirectorySetNumberOfFiles(uint8_t* name, uint8_t nameLength, uint32_t numberOfFiles) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 5;
    
    // Read file size
    union Pointer directorySize;
    
    directorySize.address = numberOfFiles;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte( &bus, fileAddress + OFFSET_DIRECTORY_SIZE + i, directorySize.byte_t[i]);
    
    return 1;
}

uint32_t fsDirectoryGetNumberOfFiles(uint8_t* name, uint8_t nameLength) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 5;
    
    // Read file size
    union Pointer directorySize;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte( &bus, fileAddress + OFFSET_DIRECTORY_SIZE + i, &directorySize.byte_t[i]);
    
    return directorySize.address;
}

uint8_t fsDirectorySetFlag(uint8_t* name, uint8_t nameLength, uint8_t flag) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 5;
    
    // Set the flag
    fs_write_byte( &bus, fileAddress + OFFSET_DIRECTORY_FLAG, flag);
    
    return 1;
}

uint8_t fsDirectoryGetFlag(uint8_t* name, uint8_t nameLength) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 5;
    
    // Get the flag
    uint8_t flag = 0;
    
    fs_read_byte( &bus, fileAddress + OFFSET_DIRECTORY_FLAG, &flag);
    
    return flag;
}