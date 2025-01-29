#ifndef __SYSCALL_VIRTUAL_FILE_SYSTEM_
#define __SYSCALL_VIRTUAL_FILE_SYSTEM_

#include <avr/io.h>

#include <kernel/kernel.h>


void vfsOpen(uint8_t* filename);
void vfsClose();
void vfsRead();
void vfsWrite();
void vfsMkdir();
void vfsRmdir();
void vfsUnlink();


struct VirtualFileSystemInterface {
    
    void(*open)(uint8_t*);     // Open or create a file
    void(*close)();            // Close a file
    
    void(*read)();             // Read data from a file
    void(*write)();            // Write data to a file
    
    void(*mkdir)();            // Create directory
    void(*rmdir)();            // Remove directory
    
    void(*unlink)();           // Deletes file
    
};

#endif
