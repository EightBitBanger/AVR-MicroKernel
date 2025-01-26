#include <kernel/fs/fs.h>

uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsFileCreate(name, nameLength, 128);
    
    if (directoryAddress == 0) 
        return 0;
    
    // Mark the file as a directory
    struct FSAttribute attrib = {' ', 'r', 'w', 'd'};
    fsFileSetAttributes(directoryAddress, &attrib);
    
    fsFileOpen(directoryAddress);
    uint32_t fileSize = fsFileGetSize(directoryAddress);
    
    uint8_t buffer[fileSize];
    fsFileRead(buffer, fileSize);
    
    for (uint32_t i=0; i < fileSize; i++) 
        buffer[i] = 0x00;
    
    fsFileWrite(buffer, fileSize);
    
    fsFileClose();
    
    return directoryAddress;
}
