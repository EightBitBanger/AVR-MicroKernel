#include <kernel/kernel.h>

uint8_t fsFileSetAttributes(uint32_t address, struct FSAttribute* attributes) {
    
    // Write file attributes
    uint8_t attributeArray[4] = {' ', ' ', ' ', ' '};
    
    attributeArray[0] = attributes->executable;
    attributeArray[1] = attributes->readable;
    attributeArray[2] = attributes->writeable;
    attributeArray[3] = attributes->type;
    
    for (uint8_t i=0; i < 4; i++) {
        fs_write_byte(address + OFFSET_FILE_ATTRIBUTES + i, attributeArray[i]);
    }
    
	return 1;
}
