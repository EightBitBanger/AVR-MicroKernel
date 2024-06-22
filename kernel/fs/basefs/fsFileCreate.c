#include <kernel/kernel.h>

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize) {
    
    uint32_t fileAddress = fsAllocate(fileSize);
	
	// Write file name
	for (uint8_t i=0; i < nameLength - 1; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_NAME, name[i]);
    
    // Set file size
    union Pointer sizePtr;
    sizePtr.address = fileSize;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_SIZE, sizePtr.byte_t[i]);
    
    // Write file attributes
    uint8_t attributes[4] = {' ', 'r', 'w', ' '};
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_ATTRIBUTES, attributes[i]);
    
    // Zero the directory size
    union Pointer dirSzPtr;
    dirSzPtr.address = 0;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + DIRECTORY_OFFSET_SIZE, dirSzPtr.byte_t[i]);
    
    // Check file claimed by a directory
    uint8_t flagClaimed = 0;
    fs_write_byte(fileAddress + DIRECTORY_OFFSET_FLAG, flagClaimed);
    
    return fileAddress;
}
