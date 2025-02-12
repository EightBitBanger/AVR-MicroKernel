#include <kernel/fs/fs.h>

uint32_t fsFileCopy(uint8_t* sourceName, uint8_t sourceLength, 
                    uint8_t* destName, uint8_t destLength, 
                    uint8_t deviceLetter, uint32_t directoryAddress) {
    if (sourceLength > FILE_NAME_LENGTH) 
        sourceLength = FILE_NAME_LENGTH;
    if (destLength > FILE_NAME_LENGTH) 
        destLength = FILE_NAME_LENGTH;
    
    // Copy source file
    uint32_t sourceAddress = fsFileExists(sourceName, sourceLength);
    
    if (sourceAddress == 0) 
        return 0;
    
    uint32_t fileSize = fsFileGetSize(sourceAddress);
    
    int32_t indexSrc = fsFileOpen(sourceAddress);
    
    uint8_t buffer[fileSize];
    
    fsFileRead(indexSrc, buffer, fileSize);
    
    fsFileClose(indexSrc);
    
    // Paste destination file
    
    uint8_t currentDeviceLetter = fsDeviceGetRootLetter();
    uint32_t currentWorkingDirectoryAddress = fsWorkingDirectoryGetAddress();
    
    if ((deviceLetter == 0) | (directoryAddress == 0)) {
        fsDeviceSetRootLetter( deviceLetter );
        
        fsWorkingDirectorySetAddress( directoryAddress );
        fsWorkingDirectorySetName( directoryAddress );
        
    }
    
    uint32_t destAddress = fsFileCreate(destName, destLength, fileSize);
    
    int32_t indexDst = fsFileOpen(destAddress);
    
    fsFileWrite(indexDst, buffer, fileSize);
    
    fsFileClose(indexDst);
    
    fsDeviceSetRootLetter( currentDeviceLetter );
    
    fsWorkingDirectorySetAddress( currentWorkingDirectoryAddress );
    fsWorkingDirectorySetName( currentWorkingDirectoryAddress );
    
    return 0;
}
