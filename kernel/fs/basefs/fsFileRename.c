#include <kernel/kernel.h>

uint8_t fsFileRename(uint8_t* name, uint8_t nameLength, uint8_t* newName, uint8_t newNameLength) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    uint8_t newFilename[FILE_NAME_LENGTH];
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
        newFilename[i] = ' ';
    
    for (uint8_t i=0; i < newNameLength; i++) 
        newFilename[i] = newName[i];
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
        fs_write_byte(fileAddress + FILE_OFFSET_NAME + i, newFilename[i]);
    
    return 1;
}
