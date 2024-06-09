#include <kernel/kernel.h>

uint32_t fsGetFileSize(uint8_t* name, uint8_t nameLength) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    if (fileAddress == 0) 
        return 0;
    
    // Read file size
    union Pointer fileSize;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(fileAddress + OFFSET_FILE_SIZE + i, &fileSize.byte_t[i]);
    
    return fileSize.address;
}
