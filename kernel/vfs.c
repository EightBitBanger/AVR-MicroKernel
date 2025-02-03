#include <kernel/vfs.h>
#include <kernel/fs/fs.h>

int32_t vfsOpen(uint8_t* filename) {
    
    uint32_t fileAddress = fsFileExists(filename, sizeof(filename));
    
    return fsFileOpen(fileAddress);
}

uint8_t vfsClose(int32_t index) {
    
    return fsFileClose(index);
}

uint8_t vfsRead(int32_t index, uint8_t buffer, uint32_t size) {
    
    return fsFileRead(index, &buffer, size);
}

uint8_t vfsWrite(int32_t index, uint8_t buffer, uint32_t size) {
    
    return fsFileWrite(index, &buffer, size);
}

uint8_t vfsChdir(uint8_t* directoryName) {
    
    return vfsLookup(directoryName);
}

uint8_t vfsMkdir(uint8_t* filename) {
    
    return 1;//fsDirectoryCreate();
}

uint8_t vfsRmdir(uint8_t* filename) {
    
    return 0;
}

uint8_t vfsUnlink(uint8_t* filename) {
    
    return 0;
}


uint8_t vfsLookup(uint8_t* path) {
    
    // Copy the path string to avoid modifying the original
    uint8_t* path_copy[128];
    strncpy((char*)path_copy, (char*)path, sizeof(path_copy) - 1);
    
    // Ensure null-termination
    path_copy[sizeof(path_copy) - 1] = (uint8_t*)'\0';
    
    // Explode the string by the delimiter
    uint8_t* token = (uint8_t*)strtok((char*)path_copy, "/");
    
    // Process each token (directory name)
    while (token != NULL) {
        
        if (fsWorkingDirectoryChange(token, strlen((char*)token)+1) == 0) 
            return 0;
        
        // Get the next token
        token = (uint8_t*)strtok(NULL, "/");
    }
    
    return 1;
}

