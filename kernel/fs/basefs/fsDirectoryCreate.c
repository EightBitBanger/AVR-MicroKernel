#include <kernel/kernel.h>


uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsFileCreate(name, nameLength, 100);
    
    if (directoryAddress == 0) 
        return 0;
    
    // Mark the file as a directory
    struct FSAttribute attrib = {' ', 'r', 'w', 'd'};
    fsFileSetAttributes(directoryAddress, &attrib);
    
    // Zero the directory size
    union Pointer directorySizePtr;
    directorySizePtr.address = 0;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(directoryAddress + i + FILE_OFFSET_REF_COUNT, directorySizePtr.byte_t[i]);
    
    return directoryAddress;
}
