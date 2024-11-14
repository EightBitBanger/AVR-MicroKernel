#include <kernel/kernel.h>

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize) {
    
    uint32_t fileAddress = fsAllocate(fileSize);
	
	if (fileAddress == 0) {
        return 0;
	}
	
	// Write file name
	fsFileSetName(fileAddress, name, nameLength);
    
    // Set file size
    union Pointer sizePtr;
    sizePtr.address = fileSize;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_SIZE, sizePtr.byte_t[i]);
    
    // Write file attributes
    struct FSAttribute attrib = {' ', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    // Zero the directory size
    union Pointer dirSzPtr;
    dirSzPtr.address = 0;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_REF_COUNT, dirSzPtr.byte_t[i]);
    
    // Clear flag byte
    fs_write_byte(fileAddress + FILE_OFFSET_FLAG, 0);
    
    return fileAddress;
}
