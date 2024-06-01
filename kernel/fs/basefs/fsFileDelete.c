#include <kernel/kernel.h>

uint8_t fsFileDelete(uint8_t* name, uint8_t nameLength) {
    
    struct Bus bus;
    bus.read_waitstate = 5;
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    uint8_t clearByte = 0x00;
    
	// Get working directory
	
	uint8_t workingDirectory[20];
    uint8_t workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
    
    uint32_t directoryAddress = 0;
	
	
	//
	// Remove the file reference from the working directory
	//
	
	if ((workingDirectoryLength > 0) & (workingDirectory[0] != ' ')) {
        
        directoryAddress = fsFileExists(workingDirectory, workingDirectoryLength);
        
        if (directoryAddress != 0) {
            
            // Get file size
            union Pointer fileSize;
            for (uint8_t i=0; i < 4; i++) 
                fs_read_byte(&bus, directoryAddress + OFFSET_FILE_SIZE + i, &fileSize.byte_t[i]);
            
            // Number of files in directory
            union Pointer directorySize;
            for (uint8_t i=0; i < 4; i++) 
                fs_read_byte(&bus, directoryAddress + OFFSET_DIRECTORY_SIZE + i, &directorySize.byte_t[i]);
            
            // Get directory file pointers
            uint8_t fileBuffer[fileSize.address];
            uint8_t index = fsFileOpen(workingDirectory, workingDirectoryLength);
            
            fsFileReadBin(index, fileBuffer, fileSize.address);
            
            for (uint32_t i=0; i < directorySize.address; i++) {
                
                // Get file pointer
                union Pointer filePtr;
                for (uint8_t f=0; f < 4; f++) 
                    filePtr.byte_t[f] = fileBuffer[(i * 4) + f];
                
                // Get file name
                uint8_t filename[10];
                for (uint8_t n=0; n < 10; n++) 
                    fs_read_byte(&bus, filePtr.address + OFFSET_FILE_NAME + n, &filename[n]);
                
                if (StringCompare(name, nameLength, filename, nameLength) == 1) {
                    
                    // Shift the last item in this ones place
                    if (i != (directorySize.address-1)) {
                        
                        for (uint8_t s=0; s < 4; s++) 
                            fileBuffer[(i * 4) + s] = fileBuffer[((directorySize.address-1) * 4) + s];
                        
                        fsFileWrite(index, fileBuffer, fileSize.address);
                        
                    }
                    
                    currentDevice = filePtr.address;
                    
                    // Decrement the file counter
                    directorySize.address--;
                    
                    for (uint8_t s=0; s < 4; s++) 
                        fs_write_byte(&bus, directoryAddress + OFFSET_DIRECTORY_SIZE + s, directorySize.byte_t[s]);
                    
                    break;
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
        if (fsGetDeviceHeaderByte( sector * SECTOR_SIZE ) != 0x55) 
            continue;
        
        uint8_t isFileFound = 0;
        if (directoryAddress == 0) {
            
            // Check file name
            for (uint8_t i=0; i < nameLength; i++) {
                
                uint8_t nameByte = 0;
                
                // Check file on root
                fs_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i, &nameByte);
                
                if (name[i] != nameByte) {
                    isFileFound = 0;
                    break;
                }
                
                isFileFound = 1;
                
                continue;
            }
            
        } else {
            
            isFileFound = 1;
            
        }
        
        // Was the file located
        if (isFileFound == 0) 
            continue;
        
        // Check file claimed by directory
        if (directoryAddress == 0) {
            
            uint8_t claimedFlag = 0x00;
            
            fs_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE) + OFFSET_DIRECTORY_FLAG, &claimedFlag);
            
            if (claimedFlag != 0) 
                continue;
            
        }
        
        uint8_t isHeaderDeleted = 0;
        
        // Delete the file sectors
        for (uint32_t nextSector = sector; nextSector < currentCapacity; nextSector++) {
            
            // Get sector header byte
            uint8_t headerByte = 0x00;
            fs_read_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), &headerByte);
            
            // Delete file header sector
            if (headerByte == 0x55) {
                
                // Only delete a header once
                if (isHeaderDeleted == 1) 
                    return 1;
                
                fs_write_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
                isHeaderDeleted = 1;
                continue;
            }
            
            // Delete data sector
            if (headerByte == 0xff) {
                
                fs_write_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
                continue;
            }
            
            // Delete end sector
            if (headerByte == 0xaa) {
                
                fs_write_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
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
