#include <stdio.h>
#include <kernel/kernel.h>

void fsFileSetFlag(uint32_t fileAddress, uint8_t index, uint8_t state) {
    
    uint8_t flag;
    
    fs_read_byte(fileAddress + FILE_OFFSET_FLAG, &flag);
    
    if (state == 1) {
        
        flag |= (1 << index);
        
    } else {
        
        flag &= ~(1 << index);
    }
    
    fs_write_byte(fileAddress + FILE_OFFSET_FLAG, flag);
    
    return;
}

uint8_t fsFileGetFlag(uint32_t fileAddress, uint8_t index) {
    
    uint8_t flag;
    
    fs_read_byte(fileAddress + FILE_OFFSET_FLAG, &flag);
    
    return (flag >> index) & 1;
}
