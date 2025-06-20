<<<<<<< HEAD
#include <kernel/fs/fs.h>

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize) {
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    uint32_t fileAddress = fsAllocate(fileSize);
	if (fileAddress == 0) 
        return 0;
	
	// Add to the current working directory
	uint32_t workingAddress = fsWorkingDirectoryGetAddress();
	if (workingAddress != 0) 
        fsDirectoryAddFile(workingAddress, fileAddress);
	
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
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_REF_COUNT, 0);
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_NEXT, 0);
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_PARENT, 0);
    
    // Clear flag byte
    fs_write_byte(fileAddress + FILE_OFFSET_FLAG, 0);
    
    return fileAddress;
}
=======
#include <kernel/fs/fs.h>

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize) {
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    uint32_t fileAddress = fsAllocate(fileSize);
	if (fileAddress == 0) 
        return 0;
	
	// Add to the current working directory
	uint32_t workingAddress = fsWorkingDirectoryGetAddress();
	if (workingAddress != 0) 
        fsDirectoryAddFile(workingAddress, fileAddress);
	
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
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_REF_COUNT, 0);
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_NEXT, 0);
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + i + FILE_OFFSET_PARENT, 0);
    
    // Clear flag byte
    fs_write_byte(fileAddress + FILE_OFFSET_FLAG, 0);
    
    return fileAddress;
}
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
