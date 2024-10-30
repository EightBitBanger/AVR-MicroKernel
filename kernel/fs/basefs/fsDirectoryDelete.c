#include <kernel/kernel.h>


uint8_t fsDirectoryDelete(uint8_t* name, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsDirectoryExists(name, nameLength);
    
    if (directoryAddress == 0) 
        return 0;
    
    // Number of files in directory
    uint32_t directoryFileCount = fsDirectoryGetNumberOfFiles(directoryAddress);
    
    for (uint32_t i=0; i < directoryFileCount; i++) {
        
        // Get file pointer
        uint32_t fileAddress = fsDirectoryGetFileAtIndex(directoryAddress, i);
        
        fsFree( fileAddress );
        
        continue;
    }
    
    // Delete the directory
    fsFileDelete(name, nameLength);
    
    return 1;
}
