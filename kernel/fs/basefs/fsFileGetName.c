#include <kernel/kernel.h>


uint8_t fsFileGetName(uint32_t fileAddress, uint8_t* name) {
    
    for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
        fs_read_byte(fileAddress + n + FILE_OFFSET_NAME, &name[n]);
    
    return 1;
}
