#include <kernel/kernel.h>

uint32_t fsDirectoryDelete(uint8_t* name, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsDirectoryExists(name, nameLength);
    
    if (directoryAddress == 0) 
        return 0;
    
    fsFree(directoryAddress);
    
    return directoryAddress;
}

uint8_t fsDirectoryDeleteContents(uint32_t directoryAddress) {
    
    // Number of files in directory
    uint32_t directoryFileCount = fsDirectoryGetNumberOfFiles(directoryAddress);
    
    for (uint32_t i=0; i < directoryFileCount; i++) 
        fsFree( fsDirectoryGetFileRef(directoryAddress, i) );
    
    return 1;
}
