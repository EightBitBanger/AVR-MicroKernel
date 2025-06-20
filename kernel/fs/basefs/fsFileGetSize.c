#include <kernel/fs/fs.h>

uint32_t fsFileGetSize(uint32_t fileAddress) {
    
    union Pointer fileSzPtr;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(fileAddress + FILE_OFFSET_SIZE + i, &fileSzPtr.byte_t[i]);
    
    return fileSzPtr.address;
}
