#include <kernel/kernel.h>

uint8_t fsFileRename(uint8_t* name, uint8_t nameLength, uint8_t* newName, uint8_t newNameLength) {
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    // Check working directory for the file
    if (fsCheckWorkingDirectory() == 1) {
        
        uint8_t workingDirectory[20];
        uint8_t workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
        
        uint32_t directoryAddress = fsFileExists(workingDirectory, workingDirectoryLength);
        
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
                union Pointer filePtr;
                for (uint8_t f=0; f < 4; f++) 
                    filePtr.byte_t[f] = fileBuffer[(i * 4) + f];
                
                // Get file name
                uint8_t filename[FILE_NAME_LENGTH];
                for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
                    filename[n] = fsSectorGetByte(filePtr.address + OFFSET_FILE_NAME + n);
                
                if (StringCompare(name, nameLength, filename, nameLength) == 1) {
                    
                    uint8_t newFilename[FILE_NAME_LENGTH];
                    
                    for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
                        newFilename[n] = ' ';
                    
                    for (uint8_t n=0; n < newNameLength; n++) 
                        newFilename[n] = newName[n];
                    
                    for (uint8_t n=0; n < newNameLength; n++) 
                        fsSectorSetByte(filePtr.address + OFFSET_FILE_NAME + n, newFilename[n]);
                    
                    return 1;
                }
                
            }
            
        }
        
        return 0;
    }
    
    // Check following sectors allocated to this file
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Find an active file start byte
        if (fsSectorGetByte( sector * SECTOR_SIZE ) != 0x55) 
            continue;
        
        // Ignore files claimed by a directory
        if (fsDirectoryGetFlag(name, nameLength) != 0) 
            continue;
        
        // Check file name
        uint8_t filename[FILE_NAME_LENGTH];
        for (uint8_t i=0; i < nameLength; i++) 
            filename[i] = fsSectorGetByte((sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i);
        
        if (StringCompare(name, nameLength, filename, nameLength) == 1) {
            
            // Replace the file name
            uint8_t newFilename[FILE_NAME_LENGTH];
            
            for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
                newFilename[n] = ' ';
            
            for (uint8_t n=0; n < newNameLength; n++) 
                newFilename[n] = newName[n];
            
            for (uint8_t i=0; i < newNameLength; i++) 
                fsSectorSetByte((sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i, newFilename[i]);
            
            return 1;
        }
        
		continue;
    }
    
    return 0;
}
