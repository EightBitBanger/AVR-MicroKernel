#include <kernel/kernel.h>

uint8_t fsGetFileAttributes(uint8_t* name, uint8_t nameLength, struct FSAttribute* attributes) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate = 5;
    
    // Write file attributes
    uint8_t attributeArray[4] = {' ', ' ', ' ', ' '};
    
    for (uint8_t i=0; i < 4; i++) {
        fs_read_byte( &bus, fileAddress + OFFSET_FILE_ATTRIBUTES + i, &attributeArray[i] );
    }
    
    attributes->executable = attributeArray[0];
    attributes->readable   = attributeArray[1];
    attributes->writeable  = attributeArray[2];
    attributes->type       = attributeArray[3];
    
    return 1;
}
