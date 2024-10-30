#include <kernel/kernel.h>

uint8_t fsFileDelete(uint8_t* name, uint8_t nameLength) {
    
    //if (fsCheckWorkingDirectory() == 0) 
    //    return 0;
    
    uint32_t fileSize = fsFileGetSize( fsWorkingDirectoryGetAddress() );
    
    // Number of files in directory
    union Pointer directorySize;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(fsWorkingDirectoryGetAddress() + FILE_OFFSET_REF_COUNT + i, &directorySize.byte_t[i]);
    
    // Get directory file pointers
    uint8_t fileBuffer[fileSize];
    fsFileOpen(fsWorkingDirectoryGetAddress());
    
    fsFileRead(fileBuffer, fileSize);
    
    for (uint32_t i=0; i < directorySize.address; i++) {
        
        // Get file pointer
        union Pointer filePtr;
        for (uint8_t f=0; f < 4; f++) 
            filePtr.byte_t[f] = fileBuffer[(i * 4) + f];
        
        // Get file name
        uint8_t filename[10];
        for (uint8_t n=0; n < 10; n++) 
            fs_read_byte(filePtr.address + FILE_OFFSET_NAME + n, &filename[n]);
        
        if (StringCompare(name, nameLength, filename, FILE_NAME_LENGTH) == 1) {
            
            // Shift the last item in this ones place
            
            // If its not the last file reference shift it up
            // The last element will just be removed
            if (i != (directorySize.address - 1)) {
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[(i * 4) + s] = fileBuffer[((directorySize.address-1) * 4) + s];
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[((directorySize.address-1) * 4) + s] = ' ';
                
            }
            
            // Remove last file reference
            if (directorySize.address == 1) {
                
                for (uint8_t s=0; s < 4; s++) 
                    fileBuffer[((directorySize.address-1) * 4) + s] = ' ';
                
            }
            
            // Decrement the file counter
            directorySize.address--;
            
            for (uint8_t s=0; s < 4; s++) 
                fs_write_byte(fsWorkingDirectoryGetAddress() + FILE_OFFSET_REF_COUNT + s, directorySize.byte_t[s]);
            
            fsFileWrite(fileBuffer, fileSize);
            
            fsFileClose();
            
            fsFree(filePtr.address);
            
            return 1;
        }
        
    }
    
    fsFileClose();
    
	return 0;
}
