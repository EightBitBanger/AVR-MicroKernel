#include <kernel/fs/fs.h>

uint32_t fsWorkingDirectoryFind(uint8_t index) {
    
    // Check if the current working directory is valid
    uint32_t directoryAddress = fsWorkingDirectoryGetAddress();
	
	if (directoryAddress == 0) 
        return 0;
	
	// Get file size
	uint32_t directorySize = fsDirectoryGetSize( directoryAddress );
    
	// Get number of files
	uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
	
	if ((index > numberOfFiles) | (numberOfFiles == 0)) 
        return 0;
    
    int32_t indexDir = fsFileOpen(directoryAddress);
    
    uint8_t bufferDir[directorySize];
    fsFileRead(indexDir, bufferDir, directorySize);
    
    fsFileClose(indexDir);
    
    // Get file address offset
    union Pointer fileAddressPtr;
    for (uint8_t p=0; p < 4; p++) 
        fileAddressPtr.byte_t[p] = bufferDir[ (index * 4) + p ];
    
    return fileAddressPtr.address;
}
