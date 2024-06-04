#include <kernel/kernel.h>

uint8_t fsFileDelete(uint8_t* name, uint8_t nameLength) {
    
    uint32_t currentDevice = fsGetDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
	// Get working directory
	
	//
	// Remove the file reference from the working directory
	//
	
	uint32_t directoryAddress = 0;
    union Pointer filePtr;
    filePtr.address = 0;
    
	if (fsCheckWorkingDirectory() == 1) {
        
        uint8_t workingDirectory[20];
        uint8_t workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
        
        directoryAddress = fsFileExists(workingDirectory, workingDirectoryLength);
        
        if (directoryAddress != 0) {
            
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
                
                // Clear the directory flag
                fsSectorSetByte(filePtr.address + OFFSET_DIRECTORY_FLAG, 0x00);
                
                if (StringCompare(name, nameLength, filename, nameLength) == 1) {
                    
                    // Shift the last item in this ones place
                    if (i != (directorySize.address-1)) {
                        
                        for (uint8_t s=0; s < 4; s++) 
                            fileBuffer[(i * 4) + s] = fileBuffer[((directorySize.address-1) * 4) + s];
                        
                        fsFileWrite(index, fileBuffer, fileSize.address);
                        
                    }
                    
                    // Decrement the file counter
                    directorySize.address--;
                    
                    for (uint8_t s=0; s < 4; s++) 
                        fsSectorSetByte(directoryAddress + OFFSET_DIRECTORY_SIZE + s, directorySize.byte_t[s]);
                    
                }
                
                continue;
            }
            
            fsFileClose(index);
            
        }
        
	}
	
	
	//
	// Delete following sectors allocated to this file
	//
	
    for (uint32_t sector=1; sector < currentCapacity; sector++) {
        
        // Find an active file start byte
        if (fsSectorGetByte( sector * SECTOR_SIZE ) != 0x55) 
            continue;
        
        // Ignore files claimed by a directory
        if (directoryAddress == 0) {
            
            uint8_t filename[FILE_NAME_LENGTH];
            for (uint8_t i=0; i < nameLength; i++) 
                filename[i] = fsSectorGetByte(currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i);
            
            if (StringCompare(filename, nameLength, name, nameLength) == 0) 
                continue;
            
            uint8_t claimedFlag = 0x00;
            
            claimedFlag = fsSectorGetByte(currentDevice + (sector * SECTOR_SIZE) + OFFSET_DIRECTORY_FLAG);
            
            if (claimedFlag != 0) 
                continue;
            
        } else {
            
            // File was not found in the working directory
            if (filePtr.address == 0) 
                return 0;
            
            currentDevice = filePtr.address;
            
            sector--;
            
        }
        
        uint8_t isHeaderDeleted = 0;
        uint8_t clearByte = 0x00;
        
        // Delete the file sectors
        for (uint32_t nextSector = sector; nextSector < currentCapacity; nextSector++) {
            
            // Get sector header byte
            uint8_t headerByte = 0x00;
            headerByte = fsSectorGetByte(currentDevice + (nextSector * SECTOR_SIZE));
            
            // Delete file header sector
            if (headerByte == 0x55) {
                
                // Only delete a header once
                if (isHeaderDeleted == 1) 
                    return 1;
                
                fsSectorSetByte(currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
                isHeaderDeleted = 1;
                continue;
            }
            
            // Delete data sector
            if (headerByte == 0xff) {
                
                fsSectorSetByte(currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
                continue;
            }
            
            // Delete end sector
            if (headerByte == 0xaa) {
                
                fsSectorSetByte(currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
                return 1;
            }
            
            // Empty sector
            if (headerByte == 0x00) 
                break;
            
            continue;
        }
        
		return 1;
    }
    
    return 0;
}