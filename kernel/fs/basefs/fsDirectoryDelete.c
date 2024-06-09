#include <kernel/kernel.h>


uint32_t fsDirectoryDelete(uint8_t* name, uint8_t nameLength) {
    
    // Delete contents of the directory...
    //
    
    // Delete the directory
    uint32_t directoryDeleted = fsFileDelete(name, nameLength);
    
    return directoryDeleted;
}
