#include <kernel/kernel.h>

extern uint32_t fs_working_directory_size;

uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength) {
    
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
        
        if (attrib.type != 'd') 
            continue;
        
        // Name
        uint8_t filename[FILE_NAME_LENGTH];
        fsFileGetName(fileAddress, filename);
        
        if (StringCompare(name, nameLength, filename, FILE_NAME_LENGTH) == 1) 
            return fileAddress;
        
        continue;
    }
    
    return 0;
}
