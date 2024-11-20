#include <kernel/kernel.h>


uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsFileCreate(name, nameLength, 128);
    
    if (directoryAddress == 0) 
        return 0;
    
    // Mark the file as a directory
    struct FSAttribute attrib = {' ', 'r', 'w', 'd'};
    fsFileSetAttributes(directoryAddress, &attrib);
    
    return directoryAddress;
}
