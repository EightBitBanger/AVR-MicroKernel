#include <kernel/kernel.h>


uint32_t fsDirectoryFileExists(uint8_t* name, uint8_t nameLength) {
    
    if (fsCheckWorkingDirectory() == 1) {
        
        uint32_t directoryAddress = fsGetWorkingDirectoryAddress();
        
        uint8_t workingDirectory[FILE_NAME_LENGTH];
        uint8_t workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles(workingDirectory, workingDirectoryLength-1);
        uint32_t fileSize      = fsDirectoryGetFileSize(workingDirectory, workingDirectoryLength-1);
        
        // Load the directory data
        uint8_t dirBuffer[fileSize];
        
        uint32_t sector = 0;
        uint32_t sectorCounter = 0;
        
        for (uint32_t i=0; i < fileSize; i++) {
            
            // Skip the sector marker byte
            if (sectorCounter == (SECTOR_SIZE - 1)) {
                sectorCounter=0;
                sector++;
            }
            
            uint32_t positionOffset = directoryAddress + SECTOR_SIZE + sector + 1;
            
            fs_read_byte(positionOffset, &dirBuffer[i]);
            
            sectorCounter++;
            sector++;
            
            continue;
        }
        
        
        for (uint32_t d=0; d < numberOfFiles; d++) {
            
            // Get the address to the file
            union Pointer fileAddress;
            for (uint8_t i=0; i < 4; i++) 
                fileAddress.byte_t[i] = dirBuffer[(d * 4) + i];
            
            // Check the filename
            uint8_t filename[FILE_NAME_LENGTH];
            for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
                fs_read_byte(fileAddress.address + OFFSET_FILE_NAME + i, &filename[i]);
            
            if (StringCompare(filename, FILE_NAME_LENGTH, name, nameLength) == 1) 
                return fileAddress.address;
            
        }
        
    }
    
    return 0;
}
