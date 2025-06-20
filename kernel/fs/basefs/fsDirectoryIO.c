<<<<<<< HEAD
#include <kernel/fs/fs.h>

uint32_t fsDirectoryGetSize(uint32_t directoryAddress) {
    union Pointer directorySizePtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + i, &directorySizePtr.byte_t[i]);
    return directorySizePtr.address;
}

uint32_t fsDirectoryGetCapacity(uint32_t directoryAddress) {
    uint32_t totalSize = 0;
    
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        totalSize += fsDirectoryGetNumberOfFiles( directoryAddress );
        
        // Get the pointer to the next directory block
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last block
        if (directoryAddress == 0) 
            break;
        
        continue;
    }
    
    return totalSize;
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


uint32_t fsDirectoryGetFile(uint32_t directoryAddress, uint32_t index) {
    
    uint32_t counter=0;
    
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            if (counter == index) {
                
                // Get file size
                uint32_t directorySize = fsDirectoryGetSize(directoryAddress);
                
                int32_t index = fsFileOpen(directoryAddress);
                
                uint8_t buffer[directorySize];
                fsFileRead(index, buffer, directorySize);
                
                fsFileClose(index);
                
                union Pointer fileAddressPtr;
                for (uint8_t p=0; p < 4; p++) 
                    fileAddressPtr.byte_t[p] = buffer[ (i * 4) + p ];
                
                return fileAddressPtr.address;
            }
            
            counter++;
            
            continue;
        }
        
        // Get the pointer to the next directory block
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last block
        if (directoryAddress == 0) 
            return 0;
        
        continue;
    }
    
    return 0;
}


uint8_t fsDirectorySetFile(uint32_t directoryAddress, uint32_t index, uint32_t fileAddress) {
    
    uint32_t counter=0;
    
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            if (counter == index) {
                
                // Get file size
                uint32_t directorySize = fsDirectoryGetSize(directoryAddress);
                
                int32_t index = fsFileOpen(directoryAddress);
                
                uint8_t buffer[directorySize];
                fsFileRead(index, buffer, directorySize);
                
                union Pointer filePtr;
                filePtr.address = fileAddress;
                
                for (uint8_t p=0; p < 4; p++) 
                    buffer[ (i * 4) + p ] = filePtr.byte_t[p];
                
                fsFileWrite(index, buffer, directorySize);
                
                fsFileClose(index);
                
                return 1;
            }
            
            counter++;
            
            continue;
        }
        
        // Get the pointer to the next directory block
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last block
        if (directoryAddress == 0) 
            return 0;
        
        continue;
    }
    
    return 0;
}


uint8_t fsDirectoryAddFile(uint32_t directoryAddress, uint32_t fileAddress) {
    
    // Get directory file size
    uint32_t directorySize = fsDirectoryGetSize( directoryAddress );
    
    // Get the number of files in the working directory
    uint32_t fileCount = fsDirectoryGetNumberOfFiles(directoryAddress);
    
    // Find the first block
    uint32_t nextAddress = fsDirectoryGetNext(directoryAddress);
    
    // Seek to the last block
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        if (nextAddress == 0) 
            break;
        
        directoryAddress = nextAddress;
        
        nextAddress = fsDirectoryGetNext(directoryAddress);
        
    }
    
    // Update directory file size
    directorySize = fsDirectoryGetSize( directoryAddress );
    
    // Update directory ref count
    fileCount = fsDirectoryGetNumberOfFiles(directoryAddress);
    
    // Allocate a new directory block
    if (fileCount >= FS_DIRECTORY_REF_MAX) {
        
        uint8_t newBlockName[] = "block";
        uint32_t newAddress = fsDirectoryCreate(newBlockName, sizeof(newBlockName));
        
        fsDirectorySetNext(directoryAddress, newAddress);
        
        directoryAddress = newAddress;
        
        fileCount = 0;
        
    }
    
    // Set parent address pointer
    fsDirectorySetParent(fileAddress, directoryAddress);
    
    // Add file reference to the directory
    int32_t index = fsFileOpen(directoryAddress);
    
    uint8_t bufferRefs[directorySize];
    
    fsFileRead(index, bufferRefs, directorySize);
    
    // Store file address as local offset
    // not scaled with the device address
    union Pointer fileAddressPtr;
    fileAddressPtr.address = fileAddress;
    
    for (uint8_t p=0; p < 4; p++) 
        bufferRefs[(fileCount * 4) + p] = fileAddressPtr.byte_t[p];
    
    // Increment the directory file counter
    fileCount++;
    
    // Set the number of files to the working directory
    fsDirectorySetNumberOfFiles(directoryAddress, fileCount);
    
    // Finalize directory buffer
    fsFileWrite(index, bufferRefs, directorySize);
    
    fsFileClose(index);
    
    return 1;
}

uint8_t fsDirectoryRemoveFile(uint32_t directoryAddress, uint32_t fileAddress) {
    
    uint32_t currentAddress = directoryAddress;
    
    for (uint32_t z = 0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles(currentAddress);
        
        for (uint8_t index = 0; index < numberOfFiles; index++) {
            
            uint32_t currentFileAddress = fsDirectoryGetFile(currentAddress, index);
            
            if (currentFileAddress != fileAddress) 
                continue;
            
            // Free the file first
            fsFree(fileAddress);
            
            // Check if the directory block can be freed
            if ((numberOfFiles - 1) == 0) {
                
                // Reroute the next/parent pointers
                uint32_t nextAddress = fsDirectoryGetNext(currentAddress);
                uint32_t parentAddress = fsDirectoryGetParent(currentAddress);
                
                if (parentAddress != 0) {
                    
                    if (nextAddress == 0) {
                        
                        fsDirectorySetNext(parentAddress, 0);
                        
                    } else {
                        
                        fsDirectorySetParent(nextAddress, parentAddress);
                        fsDirectorySetNext(parentAddress, nextAddress);
                    }
                    
                    // Free the directory block
                    // TODO fix Will delete the directory file its self and should not
                    //fsFree(currentAddress);
                }
                
            } else {
                
                // Move the last file reference to the current index
                uint32_t lastFileRef = fsDirectoryGetFile(currentAddress, numberOfFiles - 1);
                fsDirectorySetFile(currentAddress, index, lastFileRef);
                
                // Clear the last file reference
                fsDirectorySetFile(currentAddress, numberOfFiles - 1, 0);
            }
            
            // Decrement the file counter
            numberOfFiles--;
            fsDirectorySetNumberOfFiles(currentAddress, numberOfFiles);
            
            return 1;
        }
        
        // Get the pointer to the next directory block
        currentAddress = fsDirectoryGetNext(currentAddress);
        
        // Check if it's the last block
        if (currentAddress == 0) 
            return 0;
    }
    
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

void fsDirectorySetParent(uint32_t directoryAddress, uint32_t parentAddress) {
    
    union Pointer parentPtr;
    parentPtr.address = parentAddress;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(directoryAddress + FILE_OFFSET_PARENT + i, parentPtr.byte_t[i]);
    
    return;
}

void fsDirectorySetNext(uint32_t directoryAddress, uint32_t nextAddress) {
    
    union Pointer nextPtr;
    nextPtr.address = nextAddress;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(directoryAddress + FILE_OFFSET_NEXT + i, nextPtr.byte_t[i]);
    
    return;
}

=======
#include <kernel/fs/fs.h>

uint32_t fsDirectoryGetSize(uint32_t directoryAddress) {
    union Pointer directorySizePtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + i, &directorySizePtr.byte_t[i]);
    return directorySizePtr.address;
}

uint32_t fsDirectoryGetCapacity(uint32_t directoryAddress) {
    uint32_t totalSize = 0;
    
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        totalSize += fsDirectoryGetNumberOfFiles( directoryAddress );
        
        // Get the pointer to the next directory block
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last block
        if (directoryAddress == 0) 
            break;
        
        continue;
    }
    
    return totalSize;
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


uint32_t fsDirectoryGetFile(uint32_t directoryAddress, uint32_t index) {
    
    uint32_t counter=0;
    
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            if (counter == index) {
                
                // Get file size
                uint32_t directorySize = fsDirectoryGetSize(directoryAddress);
                
                int32_t index = fsFileOpen(directoryAddress);
                
                uint8_t buffer[directorySize];
                fsFileRead(index, buffer, directorySize);
                
                fsFileClose(index);
                
                union Pointer fileAddressPtr;
                for (uint8_t p=0; p < 4; p++) 
                    fileAddressPtr.byte_t[p] = buffer[ (i * 4) + p ];
                
                return fileAddressPtr.address;
            }
            
            counter++;
            
            continue;
        }
        
        // Get the pointer to the next directory block
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last block
        if (directoryAddress == 0) 
            return 0;
        
        continue;
    }
    
    return 0;
}


uint8_t fsDirectorySetFile(uint32_t directoryAddress, uint32_t index, uint32_t fileAddress) {
    
    uint32_t counter=0;
    
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            if (counter == index) {
                
                // Get file size
                uint32_t directorySize = fsDirectoryGetSize(directoryAddress);
                
                int32_t index = fsFileOpen(directoryAddress);
                
                uint8_t buffer[directorySize];
                fsFileRead(index, buffer, directorySize);
                
                union Pointer filePtr;
                filePtr.address = fileAddress;
                
                for (uint8_t p=0; p < 4; p++) 
                    buffer[ (i * 4) + p ] = filePtr.byte_t[p];
                
                fsFileWrite(index, buffer, directorySize);
                
                fsFileClose(index);
                
                return 1;
            }
            
            counter++;
            
            continue;
        }
        
        // Get the pointer to the next directory block
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last block
        if (directoryAddress == 0) 
            return 0;
        
        continue;
    }
    
    return 0;
}


uint8_t fsDirectoryAddFile(uint32_t directoryAddress, uint32_t fileAddress) {
    
    // Get directory file size
    uint32_t directorySize = fsDirectoryGetSize( directoryAddress );
    
    // Get the number of files in the working directory
    uint32_t fileCount = fsDirectoryGetNumberOfFiles(directoryAddress);
    
    // Find the first block
    uint32_t nextAddress = fsDirectoryGetNext(directoryAddress);
    
    // Seek to the last block
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        if (nextAddress == 0) 
            break;
        
        directoryAddress = nextAddress;
        
        nextAddress = fsDirectoryGetNext(directoryAddress);
        
    }
    
    // Update directory file size
    directorySize = fsDirectoryGetSize( directoryAddress );
    
    // Update directory ref count
    fileCount = fsDirectoryGetNumberOfFiles(directoryAddress);
    
    // Allocate a new directory block
    if (fileCount >= FS_DIRECTORY_REF_MAX) {
        
        uint8_t newBlockName[] = "block";
        uint32_t newAddress = fsDirectoryCreate(newBlockName, sizeof(newBlockName));
        
        fsDirectorySetNext(directoryAddress, newAddress);
        
        directoryAddress = newAddress;
        
        fileCount = 0;
        
    }
    
    // Set parent address pointer
    fsDirectorySetParent(fileAddress, directoryAddress);
    
    // Add file reference to the directory
    int32_t index = fsFileOpen(directoryAddress);
    
    uint8_t bufferRefs[directorySize];
    
    fsFileRead(index, bufferRefs, directorySize);
    
    // Store file address as local offset
    // not scaled with the device address
    union Pointer fileAddressPtr;
    fileAddressPtr.address = fileAddress;
    
    for (uint8_t p=0; p < 4; p++) 
        bufferRefs[(fileCount * 4) + p] = fileAddressPtr.byte_t[p];
    
    // Increment the directory file counter
    fileCount++;
    
    // Set the number of files to the working directory
    fsDirectorySetNumberOfFiles(directoryAddress, fileCount);
    
    // Finalize directory buffer
    fsFileWrite(index, bufferRefs, directorySize);
    
    fsFileClose(index);
    
    return 1;
}

uint8_t fsDirectoryRemoveFile(uint32_t directoryAddress, uint32_t fileAddress) {
    
    uint32_t currentAddress = directoryAddress;
    
    for (uint32_t z = 0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles(currentAddress);
        
        for (uint8_t index = 0; index < numberOfFiles; index++) {
            
            uint32_t currentFileAddress = fsDirectoryGetFile(currentAddress, index);
            
            if (currentFileAddress != fileAddress) 
                continue;
            
            // Free the file first
            fsFree(fileAddress);
            
            // Check if the directory block can be freed
            if ((numberOfFiles - 1) == 0) {
                
                // Reroute the next/parent pointers
                uint32_t nextAddress = fsDirectoryGetNext(currentAddress);
                uint32_t parentAddress = fsDirectoryGetParent(currentAddress);
                
                if (parentAddress != 0) {
                    
                    if (nextAddress == 0) {
                        
                        fsDirectorySetNext(parentAddress, 0);
                        
                    } else {
                        
                        fsDirectorySetParent(nextAddress, parentAddress);
                        fsDirectorySetNext(parentAddress, nextAddress);
                    }
                    
                    // Free the directory block
                    // TODO fix Will delete the directory file its self and should not
                    //fsFree(currentAddress);
                }
                
            } else {
                
                // Move the last file reference to the current index
                uint32_t lastFileRef = fsDirectoryGetFile(currentAddress, numberOfFiles - 1);
                fsDirectorySetFile(currentAddress, index, lastFileRef);
                
                // Clear the last file reference
                fsDirectorySetFile(currentAddress, numberOfFiles - 1, 0);
            }
            
            // Decrement the file counter
            numberOfFiles--;
            fsDirectorySetNumberOfFiles(currentAddress, numberOfFiles);
            
            return 1;
        }
        
        // Get the pointer to the next directory block
        currentAddress = fsDirectoryGetNext(currentAddress);
        
        // Check if it's the last block
        if (currentAddress == 0) 
            return 0;
    }
    
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

void fsDirectorySetParent(uint32_t directoryAddress, uint32_t parentAddress) {
    
    union Pointer parentPtr;
    parentPtr.address = parentAddress;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(directoryAddress + FILE_OFFSET_PARENT + i, parentPtr.byte_t[i]);
    
    return;
}

void fsDirectorySetNext(uint32_t directoryAddress, uint32_t nextAddress) {
    
    union Pointer nextPtr;
    nextPtr.address = nextAddress;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(directoryAddress + FILE_OFFSET_NEXT + i, nextPtr.byte_t[i]);
    
    return;
}

>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
