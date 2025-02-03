#include <avr/io.h>

#include <kernel/syscalls/alloc/new.h>

uint32_t globalAllocs = 0;
uint32_t globalAllocBytes = 0;

uint32_t procSuperBlock = 0;


uint32_t __new__(uint32_t size) {
    
    if (size == 0) 
        return 0;
    
    VirtualBegin();
    
    union Pointer ptr;
    ptr.address = fsAllocate(size);
    
    // Check to add the allocation to a process super block
    if (procSuperBlock != 0) 
        fsDirectoryAddFile(procSuperBlock, ptr.address);
    
    if (ptr.address != 0) {
        
        globalAllocs++;
        globalAllocBytes += size;
    }
    
    VirtualEnd();
    return ptr.address;
}

void __delete__(uint32_t ptr) {
    
    if (ptr == 0) 
        return;
    
    VirtualBegin();
    
    // Get allocation size
    uint8_t allocSize = fsFileGetSize(ptr);
    
    // Check to remove the allocation from a process super block
    if (procSuperBlock != 0) 
        fsDirectoryRemoveFile(procSuperBlock, ptr);
    
    if (fsFree( ptr ) != 0) {
        
        globalAllocs--;
        globalAllocBytes -= allocSize;
    }
    
    VirtualEnd();
    return;
}

uint32_t kAllocationsGetSize(void) {
    
    return globalAllocBytes;
}

