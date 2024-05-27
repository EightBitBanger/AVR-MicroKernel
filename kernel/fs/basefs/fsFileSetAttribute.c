#include <kernel/kernel.h>

#ifdef KERNEL_FILESYSTEM_BASE_FS

uint8_t fsSetFileAttributes(uint8_t* name, uint8_t nameLength, struct FSAttribute* attributes) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate = 5;
    
    // Write file attributes
    uint8_t attributeArray[4] = {' ', ' ', ' ', ' '};
    
    attributeArray[0] = attributes->executable;
    attributeArray[1] = attributes->readable;
    attributeArray[2] = attributes->writeable;
    attributeArray[3] = attributes->type;
    
    for (uint8_t i=0; i < 4; i++) {
        fs_write_byte( &bus, fileAddress + OFFSET_FILE_ATTRIBUTES + i, attributeArray[i] );
    }
    
	return 1;
}

#endif
