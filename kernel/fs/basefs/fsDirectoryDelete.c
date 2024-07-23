#include <kernel/kernel.h>


uint8_t fsDirectoryDelete(uint8_t* name, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsDirectoryExists(name, nameLength);
    
    if (directoryAddress == 0) {
        return 0;
    }
    
    // Free the contents of the directory
    
    // Number of files in directory
    union Pointer directorySize;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + DIRECTORY_OFFSET_SIZE + i, &directorySize.byte_t[i]);
    
    // Get file size
    union Pointer fileSize;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + i, &fileSize.byte_t[i]);
    
    // Get directory file pointers
    uint8_t fileBuffer[fileSize.address];
    fsFileOpen(directoryAddress);
    
    fsFileRead(fileBuffer, fileSize.address);
    
    for (uint32_t i=0; i < directorySize.address; i++) {
        
        // Get file pointer
        union Pointer filePtr;
        for (uint8_t f=0; f < 4; f++) 
            filePtr.byte_t[f] = fileBuffer[(i * 4) + f];
        
        fs_write_byte(filePtr.address + DIRECTORY_OFFSET_FLAG, 0);
        
        continue;
    }
    
    return fsFileDelete(name, nameLength);
}
