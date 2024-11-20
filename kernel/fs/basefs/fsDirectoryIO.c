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

uint8_t fsDirectoryAddFile(uint32_t directoryAddress, uint32_t fileAddress) {
    
    // Get directory file size
    uint32_t directorySize = fsDirectoryGetSize( directoryAddress );
    
    // Get the number of files in the working directory
    union Pointer fileCountPtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_REF_COUNT + i, &fileCountPtr.byte_t[i]);
    
    // Find the first block
    union Pointer nextPtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_NEXT + i, &nextPtr.byte_t[i]);
    
    // Seek to the last block
    while(nextPtr.address != 0) {
        
        directoryAddress = nextPtr.address;
        
        for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(directoryAddress + FILE_OFFSET_NEXT + i, &nextPtr.byte_t[i]);
        
    }
    
    // Update directory file size
    directorySize = fsDirectoryGetSize( directoryAddress );
    
    // Update directory ref count
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_REF_COUNT + i, &fileCountPtr.byte_t[i]);
    
    // Allocate a new directory block
    if (fileCountPtr.address >= FS_DIRECTORY_REF_MAX) {
        
        uint8_t newBlockName[] = "block";
        union Pointer newPtr;
        newPtr.address = fsDirectoryCreate(newBlockName, sizeof(newBlockName));
        
        // Set the current directory block to point to the next
        for (uint8_t i=0; i < 4; i++) 
            fs_write_byte(directoryAddress + FILE_OFFSET_NEXT + i, newPtr.byte_t[i]);
        
        directoryAddress = newPtr.address;
        
        fileCountPtr.address = 0;
        
    }
    
    // Set parent address pointer
    union Pointer parentPtr;
    parentPtr.address = directoryAddress;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + FILE_OFFSET_PARENT + i, parentPtr.byte_t[i]);
    
    //
    // Add file reference to the directory
    fsFileOpen(directoryAddress);
    
    uint8_t bufferRefs[directorySize];
    
    fsFileRead(bufferRefs, directorySize);
    
    // Store file address as local offset
    // not scaled with the device address
    union Pointer fileAddressPtr;
    fileAddressPtr.address = fileAddress;
    
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


uint8_t fsDirectoryRemoveFileRef(uint32_t directoryAddress, uint32_t fileAddress) {
    
    uint32_t fileSize = fsFileGetSize( directoryAddress );
    
    // Number of files in directory
    uint32_t directorySize;
    directorySize = fsDirectoryGetNumberOfFiles(directoryAddress);
    
    // Zero parent address pointer
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(fileAddress + FILE_OFFSET_PARENT + i, 0);
    
    // Get directory file pointers
    uint8_t fileBuffer[fileSize];
    fsFileOpen(directoryAddress);
    
    fsFileRead(fileBuffer, fileSize);
    
    for (uint32_t i=0; i < directorySize; i++) {
        
        // Get file pointer
        union Pointer filePtr;
        for (uint8_t f=0; f < 4; f++) 
            filePtr.byte_t[f] = fileBuffer[(i * 4) + f];
        
        // Check file reference
        
        if (fileAddress == filePtr.address) {
            
            // Shift the last item in this ones place
            
            // If its not the last file reference shift it up
            // The last element will just be removed
            if (i != (directorySize - 1)) {
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[(i * 4) + s] = fileBuffer[((directorySize-1) * 4) + s];
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[((directorySize-1) * 4) + s] = ' ';
                
            }
            
            // Remove last file reference
            if (directorySize == 1) {
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[((directorySize-1) * 4) + s] = ' ';
                
            }
            
            // Decrement the file counter
            directorySize--;
            fsDirectorySetNumberOfFiles(directoryAddress, directorySize);
            
            fsFileWrite(fileBuffer, fileSize);
            
            fsFileClose();
            
            return 1;
        }
        
    }
    
    fsFileClose();
    
    return 0;
}

uint32_t fsDirectoryGetParent(uint32_t directoryAddress) {
    
    union Pointer parentPtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_PARENT + i, &parentPtr.byte_t[i]);
    
    return parentPtr.address;
}

uint32_t fsDirectoryGetNext(uint32_t directoryAddress) {
    
    union Pointer nextPtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_NEXT + i, &nextPtr.byte_t[i]);
    
    return nextPtr.address;
}

