#include <kernel/kernel.h>


uint32_t fsWorkingDirectoryFind(uint8_t index) {
    
    //if (fsCheckWorkingDirectory() == 0) 
    //    return 0;
    
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
    
    fsFileOpen(directoryAddress);
    
    uint8_t bufferDir[directorySize];
    fsFileRead(bufferDir, directorySize);
    
    fsFileClose();
    
    // Get file address offset
    union Pointer fileAddressPtr;
    for (uint8_t p=0; p < 4; p++) 
        fileAddressPtr.byte_t[p] = bufferDir[ (index * 4) + p ];
    
    return fileAddressPtr.address;
}
