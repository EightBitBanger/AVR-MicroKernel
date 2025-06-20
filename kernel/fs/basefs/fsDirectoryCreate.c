#include <kernel/fs/fs.h>

uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength) {
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    uint32_t directoryAddress = fsFileCreate(name, nameLength, 64);
    
    if (directoryAddress == 0) 
        return 0;
    
    // Mark the file as a directory
    struct FSAttribute attrib = {' ', 'r', 'w', 'd'};
    fsFileSetAttributes(directoryAddress, &attrib);
    
    int32_t index = fsFileOpen(directoryAddress);
    uint32_t fileSize = fsFileGetSize(directoryAddress);
    
    uint8_t buffer[fileSize];
    fsFileRead(index, buffer, fileSize);
    
    for (uint32_t i=0; i < fileSize; i++) 
        buffer[i] = 0x00;
    
    fsFileWrite(index, buffer, fileSize);
    
    fsFileClose(index);
    
    return directoryAddress;
}
