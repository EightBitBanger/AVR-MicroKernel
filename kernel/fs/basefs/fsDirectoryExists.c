#include <kernel/kernel.h>


uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsWorkingDirectoryGetAddress();
    
    union Pointer nextPtr;
    nextPtr.address = directoryAddress;
    
    while(1) {
        
        // Get number of files
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
        
        if (numberOfFiles == 0) 
            continue;
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            // Get file address offset
            uint32_t fileAddress = fsDirectoryGetFileRef( directoryAddress, i );
            
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
        
        
        // Get the pointer to the next directory block
        for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(directoryAddress + FILE_OFFSET_NEXT + i, &nextPtr.byte_t[i]);
        
        directoryAddress = nextPtr.address;
        
        // Check last block
        if (directoryAddress == 0) 
            return 0;
        
        continue;
    }
    
    return 0;
}
