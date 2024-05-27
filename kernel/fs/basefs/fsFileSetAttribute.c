#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/fs/fs.h>

#ifdef KERNEL_FILESYSTEM_BASE_FS

uint8_t fsSetFileAttributes(uint8_t* name, uint8_t nameLength, struct FSAttribute* attributes) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    // Write file attributes
    uint8_t attributeArray[4] = {' ', ' ', ' ', ' '};
    
    if (attributes->executable != 0) attributeArray[1] = 'x';
    if (attributes->readable   != 0) attributeArray[2] = 'r';
    if (attributes->writeable  != 0) attributeArray[3] = 'w';
    
    for (uint8_t i=0; i < 4; i++) {
        fs_write_byte( &bus, fileAddress + OFFSET_FILE_ATTRIBUTES + i, attributeArray[i] );
    }
    
	return 1;
}

#endif
