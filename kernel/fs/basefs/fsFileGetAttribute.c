#include <kernel/kernel.h>

uint8_t fsFileGetAttributes(uint32_t address, struct FSAttribute* attributes) {
    
    // Write file attributes
    uint8_t attributeArray[4] = {' ', ' ', ' ', ' '};
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(address + OFFSET_FILE_ATTRIBUTES + i, &attributeArray[i]);
    
    attributes->executable = attributeArray[0];
    attributes->readable   = attributeArray[1];
    attributes->writeable  = attributeArray[2];
    attributes->type       = attributeArray[3];
    
    return 1;
}
