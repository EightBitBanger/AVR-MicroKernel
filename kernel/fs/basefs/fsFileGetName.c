#include <kernel/kernel.h>


void fsFileGetName(uint32_t fileAddress, uint8_t* name) {
    
    for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
        fs_read_byte(fileAddress + n + FILE_OFFSET_NAME, &name[n]);
    
    return;
}

void fsFileSetName(uint32_t fileAddress, uint8_t* name, uint8_t length) {
    
    for (uint8_t n=0; n < length-1; n++) 
        fs_write_byte(fileAddress + n + FILE_OFFSET_NAME, name[n]);
    
    return;
}
