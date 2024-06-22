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

struct Directory directoryStack[8];

uint8_t fs_directory_stack_ptr;



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
    
    fs_directory_stack_ptr = 0;
    
    fs_working_directory_address = 0;
    
    return;
}

uint8_t fsSetWorkingDirectory(uint8_t* directoryName, uint8_t nameLength) {
    
    uint32_t directoryAddress = fsFileExists(directoryName, nameLength);
    
    if (directoryAddress == 0) 
        return 0;
    
    if (fs_working_directory_address == directoryAddress) 
        return 0;
    
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    for (uint8_t i=0; i < nameLength + 1; i++) 
        fs_working_directory[i] = directoryName[i];
    
    fs_working_directory_length = nameLength + 1;
    
    fs_working_directory_address = directoryAddress;
    
    return 1;
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
