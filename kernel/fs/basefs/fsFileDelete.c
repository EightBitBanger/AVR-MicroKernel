#include <kernel/kernel.h>

uint32_t fsFileDelete(uint8_t* name, uint8_t nameLength) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    fsFree(fileAddress);
    
    return fileAddress;
}
