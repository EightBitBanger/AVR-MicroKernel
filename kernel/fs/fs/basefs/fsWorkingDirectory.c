#include <kernel/fs/fs.h>

uint8_t  fs_working_directory_length;
uint32_t fs_working_directory_address;

uint32_t fs_working_directory_size;

uint8_t fs_directory_stack_ptr;



uint32_t fsWorkingDirectoryGetParent(void) {
    
    union Pointer directoryParentPtr;
	for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(fs_working_directory_address + FILE_OFFSET_PARENT + i, &directoryParentPtr.byte_t[i]);
    
    return directoryParentPtr.address;
}


uint8_t fsWorkingDirectorySetToParent(void) {
    
    if (fs_directory_stack_ptr <= 1) 
        return 0;
    
    uint32_t perentDirectoryAddress = fsWorkingDirectoryGetParent();
    
    if (perentDirectoryAddress != 0) {
        
        fs_directory_stack_ptr--;
        
        fs_working_directory_length = FILE_NAME_LENGTH;
        
        fs_working_directory_address = perentDirectoryAddress;
        
        return 1;
    }
    
    return 0;
}


void fsWorkingDirectorySetStack(uint8_t amount) {
    
    fs_directory_stack_ptr = amount;
    
    return;
}

uint8_t fsWorkingDirectoryGetStack(void) {
    
    return fs_directory_stack_ptr;
}

void fsWorkingDirectoryClear(void) {
    
    fs_directory_stack_ptr = 0;
    
    fs_working_directory_address = 0;
    
    fs_working_directory_length = 0;
    
    fsWorkingDirectorySetAddress( fsDeviceGetRootDirectory() );
    fsWorkingDirectorySetName( fsDeviceGetRootDirectory() );
    
    return;
}

uint8_t fsWorkingDirectoryChange(uint8_t* directoryName, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsDirectoryExists(directoryName, nameLength-1);
    
    if (directoryAddress == 0) 
        return 0;
    
    union Pointer directorySizePtr;
	for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_REF_COUNT + i, &directorySizePtr.byte_t[i]);
    
	fs_working_directory_size = directorySizePtr.address;
    
    fs_directory_stack_ptr++;
    
    fsWorkingDirectorySetAddress(directoryAddress);
    
    return 1;
}

uint8_t fsWorkingDirectoryGetName(uint8_t* directoryName) {
    
    fsFileGetName(fsWorkingDirectoryGetAddress(), directoryName);
    
    return fs_working_directory_length;
}

uint32_t fsWorkingDirectoryGetFileCount(void) {
    
    union Pointer directoryCountPtr;
	for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(fs_working_directory_address + FILE_OFFSET_REF_COUNT + i, &directoryCountPtr.byte_t[i]);
    
    return directoryCountPtr.address;
}

uint32_t fsWorkingDirectoryGetAddress(void) {
    
    return fs_working_directory_address;
}

void fsWorkingDirectorySetAddress(uint32_t address) {
    
    fs_working_directory_address = address;
    
    return;
}

uint8_t fsWorkingDirectorySetName(uint32_t directoryAddress) {
    
    uint8_t filename[FILE_NAME_LENGTH];
    
    fsFileGetName(directoryAddress, filename);
    
    uint8_t nameLength = 0;
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
        if (filename[i] == ' ') 
        nameLength = i;
    }
    
    fs_working_directory_length = nameLength;
    
    return 1;
}
