#include <kernel/kernel.h>


uint32_t fsFileExists(uint8_t* name, uint8_t nameLength) {
    
    if ((name[0] == ' ') | 
        (nameLength == 0))
        return 0;
    
    // Check working directory
    if (fsWorkingDirectoryCheck() == 0) {
        return 0;
    }
    
	// Get number of files
	uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( fsWorkingDirectoryGetAddress() );
	
    if (numberOfFiles == 0) 
        return 0;
    
    for (uint8_t i=0; i < numberOfFiles; i++) {
        
        // Get file address offset
        uint32_t fileAddress = fsDirectoryGetFileAtIndex( fsWorkingDirectoryGetAddress(), i );
        
        // Check file is not a directory
        struct FSAttribute attrib;
        fsFileGetAttributes(fileAddress, &attrib);
        
        if (attrib.type == 'd') 
            continue;
        
        // Name
        uint8_t filename[FILE_NAME_LENGTH];
        fsFileGetName(fileAddress, filename);
        
        if (StringCompare(name, nameLength, filename, FILE_NAME_LENGTH) == 1) 
            return fileAddress;
        
        continue;
    }
    
    
    /*
    
    // Scan the entire volume for file headers
    //
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / FORMAT_SECTOR_SIZE;
    
    // Root full sweep
    for (uint32_t sector=0; sector <= currentCapacity; sector++) {
        
        uint32_t currentSector = sector * FORMAT_SECTOR_SIZE;
        
        // Find an active file start byte
        uint8_t headerByte;
        fs_read_byte(currentSector, &headerByte);
        
        if (headerByte != FORMAT_SECTOR_HEADER) 
            continue;
        
        // Check file belongs to root
        uint8_t flag = 0;
        fs_read_byte(currentSector + DIRECTORY_OFFSET_FLAG, &flag);
        
        if (flag != 0) 
            continue;
        
        // Check file is not a directory
        uint8_t directoryAttrib;
        fs_read_byte(currentSector + FILE_ATTRIBUTE_SPECIAL, &directoryAttrib);
        
        if (directoryAttrib == 'd') 
            continue;
        
        // Get filename from the device
        uint8_t filenameCheck[FILE_NAME_LENGTH];
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            fs_read_byte(currentSector + FILE_OFFSET_NAME + i, &filenameCheck[i]);
        
        if (StringCompare(name, nameLength, filenameCheck, FILE_NAME_LENGTH) == 1) 
            return currentSector;
        
        continue;
    }
    
    */
    
    return 0;
}
