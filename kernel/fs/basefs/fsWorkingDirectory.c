#include <kernel/kernel.h>

extern uint32_t fs_device_address;
extern struct Bus fs_bus;


extern uint8_t  fs_device_root;

extern uint8_t  fs_working_directory[FILE_NAME_LENGTH];
extern uint8_t  fs_working_directory_length;
extern uint32_t fs_working_directory_address;

extern uint8_t fs_directory_stack_ptr;

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

void fsSetWorkingDirectory(uint8_t* directoryName, uint8_t nameLength) {
    
    uint32_t fileAddress = fsFileExists(directoryName, nameLength);
    
    if (fileAddress == 0) 
        return;
    
    fs_working_directory_address = fileAddress;
    
    fsClearWorkingDirectory();
    
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    for (uint8_t i=0; i < nameLength + 1; i++) 
        fs_working_directory[i] = directoryName[i];
    
    fs_working_directory_length = nameLength + 1;
    return;
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

void fsSetRootDirectory(uint8_t device) {
    
    fs_device_root = device;
    
    return;
}

uint8_t fsGetRootDirectory(void) {
    
    return fs_device_root;
}

