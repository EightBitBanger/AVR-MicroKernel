#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>


uint8_t fsGetFileAttributes(uint8_t* name, uint8_t nameLength, struct FSAttribute* attributes) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    // Write file attributes
    uint8_t attributeArray[4] = {' ', ' ', ' ', ' '};
    
    attributes->executable = 0;
    attributes->readable   = 0;
    attributes->writeable  = 0;
    
    for (uint8_t i=0; i < 4; i++) {
        fs_read_byte( &bus, fileAddress + OFFSET_FILE_ATTRIBUTES + i, &attributeArray[i] );
    }
    
    if (attributeArray[1] == 'x') attributes->executable = 1;
    if (attributeArray[2] == 'r') attributes->readable   = 1;
    if (attributeArray[3] == 'w') attributes->writeable  = 1;
    
    return 1;
}

