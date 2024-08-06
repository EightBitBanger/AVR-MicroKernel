#include <kernel/kernel.h>


// Current working directory
uint8_t  fs_working_directory[FILE_NAME_LENGTH];
uint8_t  fs_working_directory_length;
uint32_t fs_working_directory_address;

uint8_t fs_directory_stack_ptr;


// Working path
struct Directory {
    
    uint8_t name[FILE_NAME_LENGTH];
    
    uint32_t address;
    
};

struct Directory directoryStack[WORKNG_DIRECTORY_STACK_SIZE];

uint8_t fs_directory_stack_ptr;


uint32_t fsDirectoryGetParent(void) {
    
    if (fs_directory_stack_ptr > 1) 
        return directoryStack[fs_directory_stack_ptr - 1].address;
    
    return 0;
}

uint8_t fsSetWorkingDirectoryToParent(void) {
    
    if (fs_directory_stack_ptr > 1) {
        
        fs_directory_stack_ptr--;
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            fs_working_directory[i] = ' ';
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            fs_working_directory[i] = directoryStack[fs_directory_stack_ptr].name[i];
        
        fs_working_directory_length = FILE_NAME_LENGTH;
        
        fs_working_directory_address = directoryStack[fs_directory_stack_ptr].address;
        
        return 1;
    }
    
    fs_directory_stack_ptr = 0;
    
    return 0;
}

uint8_t fsCheckWorkingDirectory(void) {
    
    if ((fs_working_directory_length > 0) & (fs_working_directory[0] != ' ')) 
        return 1;
    
    return 0;
}

void fsSetDirectoryStack(uint8_t amount) {
    
    fs_directory_stack_ptr = amount;
    
    return;
}

uint8_t fsGetDirectoryStack(void) {
    
    return fs_directory_stack_ptr;
}

void fsClearWorkingDirectory(void) {
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
        fs_working_directory[i] = ' ';
    
    fs_working_directory_length = 0;
    
    fs_working_directory_address = 0;
    
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

uint8_t fsChangeWorkingDirectory(uint8_t* directoryName, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsDirectoryExists(directoryName, nameLength-1);
    
    if (directoryAddress == 0) 
        return 0;
    
    return fsSetWorkingDirectory(directoryAddress);
}

uint8_t fsGetWorkingDirectory(uint8_t* directoryName) {
    
    for (uint8_t i=0; i < fs_working_directory_length; i++) 
        directoryName[i] = fs_working_directory[i];
    
    return fs_working_directory_length;
}

uint8_t fsGetWorkingDirectoryLength(void) {
    
    return fs_working_directory_length;
}

uint32_t fsGetWorkingDirectoryAddress(void) {
    
    return fs_working_directory_address;
}

void fsSetWorkingDirectoryAddress(uint32_t address) {
    
    fs_working_directory_address = address;
    
    return;
}
