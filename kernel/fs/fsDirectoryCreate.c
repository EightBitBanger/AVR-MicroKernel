#include <kernel/kernel.h>


uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize) {
    
    uint32_t fileAddress = fsFileCreate(name, nameLength, fileSize, 'd');
    
    if (fileAddress == 0) 
        return 0;
    
    // Zero the directory size
    union Pointer dirSzPtr;
    dirSzPtr.address = 0;
    
    for (uint8_t i=0; i < 4; i++) 
        fsSectorSetByte(fileAddress + i + OFFSET_DIRECTORY_SIZE, dirSzPtr.byte_t[i]);
    
    return fileAddress;
}
