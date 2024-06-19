#include <kernel/kernel.h>


uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize, uint8_t subType) {
    
    // Get working directory
	uint8_t workingDirectory[20];
    uint8_t workingDirectoryLength = 0;
    
    uint32_t directoryAddress = 0;
	
	if (fsCheckWorkingDirectory() == 1) {
        
        workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
        
        directoryAddress = fsFileExists(workingDirectory, workingDirectoryLength-1);
        
	}
	
	uint32_t fileAddress = fsSectorAllocate(fileSize);
	
	// Write file name
	for (uint8_t i=0; i < nameLength; i++) 
        fsSectorSetByte(fileAddress + i + OFFSET_FILE_NAME, name[i]);
    
    // Set file size
    union Pointer sizePtr;
    sizePtr.address = fileSize;
    
    for (uint8_t i=0; i < 4; i++) 
        fsSectorSetByte(fileAddress + i + OFFSET_FILE_SIZE, sizePtr.byte_t[i]);
    
    // Write file attributes
    uint8_t attributes[4] = {' ', 'r', 'w', subType};
    for (uint8_t i=0; i < 4; i++) 
        fsSectorSetByte(fileAddress + i + OFFSET_FILE_ATTRIBUTES, attributes[i]);
    
    // Zero the directory size
    union Pointer dirSzPtr;
    dirSzPtr.address = 0;
    
    for (uint8_t i=0; i < 4; i++) 
        fsSectorSetByte(fileAddress + i + OFFSET_DIRECTORY_SIZE, dirSzPtr.byte_t[i]);
    
    // Check file claimed by a directory
    uint8_t flagClaimed = 0;
    
    if (directoryAddress != 0) {
        flagClaimed = 1;
        
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles(workingDirectory, workingDirectoryLength-1);
        
        // Add file reference to the directory
        
        uint32_t directorySize = fsGetFileSize(workingDirectory, workingDirectoryLength-1);
        
        uint8_t index = fsFileOpen(workingDirectory, workingDirectoryLength-1);
        
        uint8_t bufferRefs[directorySize];
        
        for (uint8_t i=0; i < directorySize; i++) 
            bufferRefs[i] = ' ';
        
        fsFileReadBin(index, bufferRefs, directorySize);
        
        union Pointer fileAddressPtr;
        fileAddressPtr.address = fileAddress;
        
        for (uint8_t p=0; p < 4; p++) 
            bufferRefs[(numberOfFiles * 4) + p] = fileAddressPtr.byte_t[p];
        
        // Increment the directory file counter
        numberOfFiles++;
        fsDirectorySetNumberOfFiles(workingDirectory, workingDirectoryLength-1, numberOfFiles);
        
        fsFileWrite(index, bufferRefs, directorySize);
        
        fsFileClose(index);
    }
    
    // FLAG Claimed by a directory
    fsSectorSetByte(fileAddress + OFFSET_DIRECTORY_FLAG, flagClaimed);
    
    return fileAddress;
}
