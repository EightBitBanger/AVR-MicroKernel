#include <kernel/kernel.h>

uint32_t fsDirectoryGetSize(uint32_t directoryAddress) {
    
    union Pointer directorySizePtr;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + i, &directorySizePtr.byte_t[i]);
    
    return directorySizePtr.address;
}
