#include <kernel/kernel.h>

uint32_t fsDirectoryGetSize(uint32_t directoryAddress) {
    
    union Pointer directorySizePtr;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + i, &directorySizePtr.byte_t[i]);
    
    return directorySizePtr.address;
}

uint32_t fsDirectoryGetNumberOfFiles(uint32_t directoryAddress) {
    
    union Pointer directorySizePtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_REF_COUNT + i, &directorySizePtr.byte_t[i]);
    
    return directorySizePtr.address;
}

void fsDirectorySetNumberOfFiles(uint32_t directoryAddress, uint32_t numberOfFiles) {
    
    union Pointer directorySizePtr;
    directorySizePtr.address = numberOfFiles;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(directoryAddress + FILE_OFFSET_REF_COUNT + i, directorySizePtr.byte_t[i]);
    
    return;
}


uint32_t fsDirectoryGetFileRef(uint32_t directoryAddress, uint32_t index) {
    
	// Get file size
	uint32_t directorySize = fsDirectoryGetSize(directoryAddress);
    
    if (directorySize == 0) 
        return 0;
    
    if (fsFileOpen(directoryAddress) == 0) 
        return 0;
    
    uint8_t buffer[directorySize];
    fsFileRead(buffer, directorySize);
    
    fsFileClose();
    
    union Pointer fileAddressPtr;
    
    // Get file address offset
    for (uint8_t p=0; p < 4; p++) 
        fileAddressPtr.byte_t[p] = buffer[ (index * 4) + p ];
    
    return fileAddressPtr.address;
}

uint8_t fsDirectoryAddFileRef(uint32_t directoryAddress, uint32_t refAddress) {
    
    // Get the number of files in the working directory
    union Pointer fileCountPtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_REF_COUNT + i, &fileCountPtr.byte_t[i]);
    
    // Get directory file size
    uint32_t directorySize = fsDirectoryGetSize( directoryAddress );
    
    //
    // Add file reference to the directory
    fsFileOpen(directoryAddress);
    
    uint8_t bufferRefs[directorySize];
    
    fsFileRead(bufferRefs, directorySize);
    
    // Store file address as local offset
    // not scaled to a device address
    union Pointer fileAddressPtr;
    fileAddressPtr.address = refAddress;
    
    for (uint8_t p=0; p < 4; p++) 
        bufferRefs[(fileCountPtr.address * 4) + p] = fileAddressPtr.byte_t[p];
    
    // Increment the directory file counter
    fileCountPtr.address++;
    
    // Set the number of files to the working directory
    fsDirectorySetNumberOfFiles(directoryAddress, fileCountPtr.address);
    
    // Finalize directory buffer
    fsFileWrite(bufferRefs, directorySize);
    
    fsFileClose();
    
    return 1;
}


uint8_t fsDirectoryRemoveFileRef(uint32_t directoryAddress, uint32_t refAddress) {
    
    uint32_t fileSize = fsFileGetSize( directoryAddress );
    
    // Number of files in directory
    union Pointer directorySize;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_REF_COUNT + i, &directorySize.byte_t[i]);
    
    // Get directory file pointers
    uint8_t fileBuffer[fileSize];
    fsFileOpen(fsWorkingDirectoryGetAddress());
    
    fsFileRead(fileBuffer, fileSize);
    
    for (uint32_t i=0; i < directorySize.address; i++) {
        
        // Get file pointer
        union Pointer filePtr;
        for (uint8_t f=0; f < 4; f++) 
            filePtr.byte_t[f] = fileBuffer[(i * 4) + f];
        
        // Check file reference
        
        if (refAddress == filePtr.address) {
            
            // Shift the last item in this ones place
            
            // If its not the last file reference shift it up
            // The last element will just be removed
            if (i != (directorySize.address - 1)) {
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[(i * 4) + s] = fileBuffer[((directorySize.address-1) * 4) + s];
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[((directorySize.address-1) * 4) + s] = ' ';
                
            }
            
            // Remove last file reference
            if (directorySize.address == 1) {
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[((directorySize.address-1) * 4) + s] = ' ';
                
            }
            
            // Decrement the file counter
            directorySize.address--;
            
            for (uint8_t s=0; s < 4; s++) 
                fs_write_byte(fsWorkingDirectoryGetAddress() + FILE_OFFSET_REF_COUNT + s, directorySize.byte_t[s]);
            
            fsFileWrite(fileBuffer, fileSize);
            
            fsFileClose();
            
            return 1;
        }
        
    }
    
    fsFileClose();
    
    return 0;
}

