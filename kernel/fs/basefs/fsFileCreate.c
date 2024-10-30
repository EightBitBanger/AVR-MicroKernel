#include <kernel/kernel.h>

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize) {
    
    uint32_t fileAddress = fsAllocate(fileSize);
	
	if (fileAddress == 0) {
        return 0;
	}
	
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
        fs_write_byte(fileAddress + i + FILE_OFFSET_REF_COUNT, dirSzPtr.byte_t[i]);
    
    // Get the number of files in the working directory
    union Pointer fileCountPtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(fsWorkingDirectoryGetAddress() + FILE_OFFSET_REF_COUNT + i, &fileCountPtr.byte_t[i]);
    
    // Get directory file size
    uint32_t directorySize = fsDirectoryGetSize( fsWorkingDirectoryGetAddress() );
    
    //
    // Add file reference to the directory
    
    fsFileOpen(fsWorkingDirectoryGetAddress());
    
    uint8_t bufferRefs[directorySize];
    
    fsFileRead(bufferRefs, directorySize);
    
    // Store file address as local offset
    // not scaled to a device address
    union Pointer fileAddressPtr;
    fileAddressPtr.address = fileAddress;
    
    for (uint8_t p=0; p < 4; p++) 
        bufferRefs[(fileCountPtr.address * 4) + p] = fileAddressPtr.byte_t[p];
    
    // Increment the directory file counter
    fileCountPtr.address++;
    
    // Set the number of files to the working directory
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fsWorkingDirectoryGetAddress() + FILE_OFFSET_REF_COUNT + i, fileCountPtr.byte_t[i]);
    
    // Finalize directory buffer
    fsFileWrite(bufferRefs, directorySize);
    
    fsFileClose();
    
    // Clear flag byte
    fs_write_byte(fileAddress + FILE_OFFSET_FLAG, 0);
    
    return fileAddress;
}
