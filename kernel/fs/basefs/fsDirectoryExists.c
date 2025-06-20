<<<<<<< HEAD
#include <kernel/fs/fs.h>

uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength) {
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    uint32_t directoryAddress = fsWorkingDirectoryGetAddress();
    
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        // Get number of files
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            // Get file address offset
            uint32_t fileAddress = fsDirectoryGetFile( directoryAddress, i );
            
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
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last block
        if (directoryAddress == 0) 
            return 0;
        
        continue;
    }
    
    return 0;
}
=======
#include <kernel/fs/fs.h>

uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength) {
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    uint32_t directoryAddress = fsWorkingDirectoryGetAddress();
    
    for (uint32_t z=0; z < FS_DIRECTORY_LISTING_MAX; z++) {
        
        // Get number of files
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles( directoryAddress );
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            // Get file address offset
            uint32_t fileAddress = fsDirectoryGetFile( directoryAddress, i );
            
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
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last block
        if (directoryAddress == 0) 
            return 0;
        
        continue;
    }
    
    return 0;
}
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
