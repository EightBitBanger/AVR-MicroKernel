#include <avr/io.h>

#include <kernel/syscalls/alloc/new.h>

uint32_t globalAllocs = 0;
uint32_t globalAllocBytes = 0;


uint32_t __new__(uint32_t size) {
    
    if (size == 0) 
        return 0;
    
    VirtualBegin();
    
    union Pointer ptr;
    ptr.address = fsAllocate(size);
    
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

