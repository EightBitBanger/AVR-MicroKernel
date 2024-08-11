#include <kernel/kernel.h>


uint32_t fsWorkingDirectoryFind(uint8_t index) {
    
    if (fsCheckWorkingDirectory() == 1) {
        
        // Check if the current working directory is valid
        uint32_t directoryAddress = fsWorkingDirectoryGetAddress();
		
		if (directoryAddress == 0) 
            return 0;
		
		// Get file size
		union Pointer fileSizePtr;
		for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + i, &fileSizePtr.byte_t[i]);
        
		// Get number of files
		union Pointer directorySizePtr;
		for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(directoryAddress + DIRECTORY_OFFSET_SIZE + i, &directorySizePtr.byte_t[i]);
        
		uint32_t numberOfFiles = directorySizePtr.address;
		
		if (index > numberOfFiles) 
            return 0;
        
        if (numberOfFiles == 0) 
            return 0;
        
        fsFileOpen(directoryAddress);
        
        uint8_t bufferDir[fileSizePtr.address];
        fsFileRead(bufferDir, fileSizePtr.address);
        
        fsFileClose();
        
        // Get file address offset
        union Pointer fileAddressPtr;
        for (uint8_t p=0; p < 4; p++) 
            fileAddressPtr.byte_t[p] = bufferDir[ (index * 4) + p ];
        
        return fileAddressPtr.address;
    }
    
    return 0;
}
