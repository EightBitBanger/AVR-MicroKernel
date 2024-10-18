#include <kernel/kernel.h>

extern uint32_t fs_working_directory_size;

uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength) {
    
    if ((name[0] == ' ') | 
        (nameLength == 0))
        return 0;
    
    // Check working directory
    if (fsCheckWorkingDirectory() == 1) {
        
        // Get file size
		union Pointer fileSizePtr;
		for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(fsWorkingDirectoryGetAddress() + FILE_OFFSET_SIZE + i, &fileSizePtr.byte_t[i]);
        
		uint32_t directorySize = fileSizePtr.address;
		
		// Get number of files
		union Pointer directorySizePtr;
		for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(fsWorkingDirectoryGetAddress() + DIRECTORY_OFFSET_SIZE + i, &directorySizePtr.byte_t[i]);
        
		uint32_t numberOfFiles = directorySizePtr.address;
		
        if (numberOfFiles > 0) {
            
            fsFileOpen(fsWorkingDirectoryGetAddress());
            
            uint8_t bufferDir[directorySize];
            fsFileRead(bufferDir, directorySize);
            
            fsFileClose();
            
            // List the directory contents
            for (uint8_t i=0; i < numberOfFiles; i++) {
                
                // Get file address offset
                union Pointer fileAddressPtr;
                for (uint8_t p=0; p < 4; p++) 
                    fileAddressPtr.byte_t[p] = bufferDir[ (i * 4) + p ];
                
                uint32_t fileAddress = fileAddressPtr.address;
                
                // Check file is not a directory
                uint8_t directoryAttrib;
                fs_read_byte(fileAddress + FILE_ATTRIBUTE_SPECIAL, &directoryAttrib);
                
                if (directoryAttrib != 'd') 
                    continue;
                
                // Attributes
                uint8_t attributes[4] = {' ',' ',' ',' '};
                
                for (uint8_t a=0; a < 4; a++) 
                    fs_read_byte(fileAddress + a + FILE_OFFSET_ATTRIBUTES, &attributes[a]);
                
                // Name
                uint8_t filename[FILE_NAME_LENGTH];
                
                for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
                    fs_read_byte(fileAddress + n + 1, &filename[n]);
                
                if (StringCompare(name, nameLength, filename, FILE_NAME_LENGTH) == 1) 
                    return fileAddress;
                
                continue;
            }
            
        }
        
        return 0;
    }
    
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    // Root full sweep
    for (uint32_t sector=0; sector <= currentCapacity; sector++) {
        
        uint32_t currentSector = sector * SECTOR_SIZE;
        
        // Find an active file start byte
        uint8_t headerByte;
        fs_read_byte(currentSector, &headerByte);
        
        if (headerByte != SECTOR_HEADER) 
            continue;
        
        // Check file belongs to root
        uint8_t flag = 0;
        fs_read_byte(currentSector + DIRECTORY_FLAG, &flag);
        
        if (flag != 0) 
            continue;
        
        // Check file is not a directory
        uint8_t directoryAttrib;
        fs_read_byte(currentSector + FILE_ATTRIBUTE_SPECIAL, &directoryAttrib);
        
        if (directoryAttrib != 'd') 
            continue;
        
        // Get filename from the device
        uint8_t filenameCheck[FILE_NAME_LENGTH];
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            fs_read_byte(currentSector + FILE_OFFSET_NAME + i, &filenameCheck[i]);
        
        if (StringCompare(name, nameLength, filenameCheck, FILE_NAME_LENGTH) == 1) 
            return currentSector;
        
        continue;
    }
    
    return 0;
}
