#include <kernel/kernel.h>


// Current working directory
uint8_t  fs_working_directory[FILE_NAME_LENGTH];
uint8_t  fs_working_directory_length;
uint32_t fs_working_directory_address;

uint32_t fs_working_directory_size;

uint8_t fs_directory_stack_ptr;


// Working path
struct Directory {
    
    uint8_t name[FILE_NAME_LENGTH];
    
    uint32_t address;
    
};

struct Directory directoryStack[WORKNG_DIRECTORY_STACK_SIZE];

uint8_t fs_directory_stack_ptr;


uint32_t fsWorkingDirectoryGetParent(void) {
    
    if (fs_directory_stack_ptr > 1) 
        return directoryStack[fs_directory_stack_ptr - 1].address;
    
    return 0;
}


uint8_t fsWorkingDirectorySetToParent(void) {
    
    if (fs_directory_stack_ptr > 1) {
        
        fs_directory_stack_ptr--;
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            fs_working_directory[i] = directoryStack[fs_directory_stack_ptr-1].name[i];
        
        fs_working_directory_length = FILE_NAME_LENGTH;
        
        fs_working_directory_address = directoryStack[fs_directory_stack_ptr-1].address;
        
        return 1;
    }
    
    return 0;
}


uint8_t fsWorkingDirectoryCheck(void) {
    
    if ((fs_working_directory_length > 0) & (fs_working_directory[0] != ' ')) 
        return 1;
    
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
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
        fs_working_directory[i] = ' ';
    
    fs_directory_stack_ptr = 0;
    
    fs_working_directory_address = 0;
    
    fs_working_directory_length = 0;
    
    fsSetWorkingDirectory( fsDeviceGetRootDirectory() );
    
    return;
}

uint8_t fsSetWorkingDirectory(uint32_t directoryAddress) {
    
    if (directoryAddress == 0) 
        return 0;
    
    if (fs_working_directory_address == directoryAddress) 
        return 0;
    
    uint8_t nameLength = 0;
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
        fs_read_byte(directoryAddress + FILE_OFFSET_NAME + i, &fs_working_directory[i]);
        nameLength = i;
    }
    
    fs_working_directory_length = nameLength;
    
    fs_working_directory_address = directoryAddress;
    
    return 1;
}

uint8_t fsWorkingDirectoryChange(uint8_t* directoryName, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsDirectoryExists(directoryName, nameLength-1);
    
    if (directoryAddress == 0) 
        return 0;
    
    // Add the directory to the directory stack
    for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
        directoryStack[fs_directory_stack_ptr].name[n] = ' ';
    for (uint8_t n=0; n < nameLength + 1; n++) 
        directoryStack[fs_directory_stack_ptr].name[n] = directoryName[n];
    
    directoryStack[fs_directory_stack_ptr].address = directoryAddress;
    
    union Pointer directorySizePtr;
	for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(directoryAddress + FILE_OFFSET_REF_COUNT + i, &directorySizePtr.byte_t[i]);
    
	fs_working_directory_size = directorySizePtr.address;
    
    fs_directory_stack_ptr++;
    
    return fsSetWorkingDirectory(directoryAddress);
}

uint8_t fsGetWorkingDirectory(uint8_t* directoryName) {
    
    for (uint8_t i=0; i < fs_working_directory_length; i++) 
        directoryName[i] = fs_working_directory[i];
    
    return fs_working_directory_length;
}

uint8_t fsWorkingDirectoryGetLength(void) {
    
    return fs_working_directory_length;
}

void fsWorkingDirectorySetLength(uint8_t length) {
    
    fs_working_directory_length = length;
    
    return;
}

uint32_t fsWorkingDirectoryGetFileCount(void) {
    
    return fs_working_directory_size;
}

uint32_t fsWorkingDirectoryGetAddress(void) {
    
    return fs_working_directory_address;
}

void fsWorkingDirectorySetAddress(uint32_t address) {
    
    fs_working_directory_address = address;
    
    return;
}
