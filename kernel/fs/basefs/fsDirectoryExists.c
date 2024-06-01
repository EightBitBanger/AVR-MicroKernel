#include <kernel/kernel.h>

uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsFileExists(name, nameLength);
    
    if (directoryAddress != 0) {
        
        uint32_t fileAddress = 0;
        
        fsGetFileSize
        
        
        
        
        
        return fileAddress;
    }
    
    return 0;
}
