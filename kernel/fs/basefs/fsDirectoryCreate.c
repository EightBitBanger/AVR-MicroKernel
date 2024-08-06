#include <kernel/kernel.h>


uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength) {
    
    uint32_t fileAddress = fsFileCreate(name, nameLength, 100);
    
    if (fileAddress == 0) {
        return 0;
    }
    
    // Mark the file as a directory
    fs_write_byte(fileAddress + FILE_ATTRIBUTE_SPECIAL, 'd');
    
    // Zero the directory size
    union Pointer directorySizePtr;
    directorySizePtr.address = 0;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + DIRECTORY_OFFSET_SIZE, directorySizePtr.byte_t[i]);
    
    return fileAddress;
}
