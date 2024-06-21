#include <kernel/kernel.h>

uint8_t fsFileDelete(uint8_t* name, uint8_t nameLength) {
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
	// Get working directory
	union Pointer filePtr;
    filePtr.address = 0;
    
	if (fsCheckWorkingDirectory() == 1) {
        
        uint8_t workingDirectory[20];
        uint8_t workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
        
        uint32_t directoryAddress = fsFileExists(workingDirectory, workingDirectoryLength);
        
        // Get file size
        union Pointer fileSize;
        for (uint8_t i=0; i < 4; i++) 
            fileSize.byte_t[i] = fsSectorGetByte(directoryAddress + OFFSET_FILE_SIZE + i);
        
        // Number of files in directory
        union Pointer directorySize;
        for (uint8_t i=0; i < 4; i++) 
            directorySize.byte_t[i] = fsSectorGetByte(directoryAddress + OFFSET_DIRECTORY_SIZE + i);
        
        // Get directory file pointers
        uint8_t fileBuffer[fileSize.address];
        uint8_t index = fsFileOpen(workingDirectory, workingDirectoryLength);
        
        fsFileReadBin(index, fileBuffer, fileSize.address);
        
        for (uint32_t i=0; i < directorySize.address; i++) {
            
            // Get file pointer
            for (uint8_t f=0; f < 4; f++) 
                filePtr.byte_t[f] = fileBuffer[(i * 4) + f];
            
            // Get file name
            uint8_t filename[10];
            for (uint8_t n=0; n < 10; n++) 
                filename[n] = fsSectorGetByte(filePtr.address + OFFSET_FILE_NAME + n);
            
            if (StringCompare(name, nameLength, filename, nameLength) == 1) {
                
                // Shift the last item in this ones place
                
                // If its not the last file reference shift it up
                // The last element will just be removed
                if (i != (directorySize.address - 1)) {
                    
                    for (uint8_t s=0; s < 4; s++) 
                        fileBuffer[(i * 4) + s] = fileBuffer[((directorySize.address-1) * 4) + s];
                    
                    for (uint8_t s=0; s < 4; s++) 
                        fileBuffer[((directorySize.address-1) * 4) + s] = ' ';
                    
                }
                
                
                // Decrement the file counter
                directorySize.address--;
                
                for (uint8_t s=0; s < 4; s++) {
                    
                    fsSectorSetByte(directoryAddress + OFFSET_DIRECTORY_SIZE + s, directorySize.byte_t[s]);
                    
                    break;
                }
                
                continue;
            }
            
            fsFileWrite(index, fileBuffer, fileSize.address);
            
            fsFileClose(index);
            
        }
        
	}
	
	// Destroy the file
	if (filePtr.address > 0) 
        return fsSectorDeallocate(filePtr.address);
	
    return 0;
}
