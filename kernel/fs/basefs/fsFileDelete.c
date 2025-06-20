<<<<<<< HEAD
#include <kernel/fs/fs.h>

uint32_t fsFileDelete(uint8_t* name, uint8_t nameLength) {
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    if (fileAddress == 0) 
        return 0;
    
    uint32_t workingAddress = fsWorkingDirectoryGetAddress();
	if (workingAddress != 0) 
        fsDirectoryRemoveFile(fsWorkingDirectoryGetAddress(), fileAddress);
    
    fsFree(fileAddress);
    return fileAddress;
}
=======
#include <kernel/fs/fs.h>

uint32_t fsFileDelete(uint8_t* name, uint8_t nameLength) {
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    if (fileAddress == 0) 
        return 0;
    
    uint32_t workingAddress = fsWorkingDirectoryGetAddress();
	if (workingAddress != 0) 
        fsDirectoryRemoveFile(fsWorkingDirectoryGetAddress(), fileAddress);
    
    fsFree(fileAddress);
    return fileAddress;
}
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
