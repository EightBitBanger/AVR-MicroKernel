#include <fs/fs.h>
#include <stdio.h>

uint8_t vfsList(struct Partition part, DirectoryHandle handle) {
    
    uint32_t directoryRefTotal = fsDirectoryGetTotalSize(part, handle);
    if (directoryRefTotal == 0) 
        return 0;
    
    for (uint32_t i=0; i < directoryRefTotal; i++) {
        
        uint32_t entryAddress = fsDirectoryFindByIndex(part, handle, i);
        
        uint8_t filename[] = "          \n";
        fsFileGetName(part, entryAddress, filename);
        
        printf((char*)filename);
        
    }
    
    /*
    uint8_t buffer[256];
    
    while (1) {
        uint32_t refCount = fsDirectoryGetReferenceCount(part, handle);
        
        File index = fsFileOpen(part, handle);
        uint32_t directorySize = fsFileGetSize(part, handle);
        
        fsFileRead(part, index, buffer, directorySize);
        
        // Run file list
        for (uint32_t i=0; i < refCount; i++) {
            uint8_t ptrBytes[4];
            for (uint8_t a=0; a < 4; a++) 
                ptrBytes[a] = buffer[ (i * 4) + a ];
            
            uint32_t fileHandle = *((uint32_t*)&ptrBytes[0]);
            
            if (fileHandle == 0) 
                continue;
            
            uint32_t fileSize = fsFileGetSize(part, fileHandle);
            
            // Print attributes
            uint8_t fileAttr[] = "    ";
            fsFileGetAttributes(part, fileHandle, fileAttr);
            
            // Check directory or file
            uint8_t isDirectory = 0;
            if (fileAttr[3] == 'd') 
                isDirectory = 1;
            
            fileAttr[3] = '\0';
            printf((char*)fileAttr);
            printf(" ");
            
            // Print file name
            uint8_t filename[] = "           ";
            fsFileGetName(part, fileHandle, filename);
            printf((char*)filename);
            
            // Print file size
            if (isDirectory == 0) {
                printf("%u", fileSize);
                printf("\n");
            } else {
                printf("<DIR>\n");
            }
            
        }
        fsFileClose(index);
        
        handle = fsFileGetNextAddress(part, handle);
        if (handle == 0) 
            break;
    }
    */
    return 0;
}
