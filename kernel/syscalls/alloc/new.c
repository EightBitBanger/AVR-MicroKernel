#include <avr/io.h>

#include <kernel/syscalls/alloc/new.h>

uint32_t globalAllocs = 0;
uint32_t globalAllocBytes = 0;


uint32_t __new__(uint32_t size) {
    
    if (size == 0) 
        return 0;
    
    uint8_t deviceLetter = fsDeviceGetRoot();
    fsDeviceSetLetter('x');
    
    union Pointer ptr;
    ptr.address = fsAllocate(size);
    
    if (ptr.address != 0) {
        
        globalAllocs++;
        globalAllocBytes += size;
        
    }
    
    fsDeviceSetLetter(deviceLetter);
    return ptr.address;
}

void __delete__(uint32_t ptr) {
    
    if (ptr == 0) 
        return;
    
    uint8_t deviceLetter = fsDeviceGetRoot();
    fsDeviceSetLetter('x');
    
    // Get allocation size
    uint8_t allocSize = fsFileGetSize(ptr);
    
    if (fsFree( ptr ) != 0) {
        
        globalAllocs--;
        globalAllocBytes -= allocSize;
        
    }
    
    fsDeviceSetLetter(deviceLetter);
    return;
}

uint32_t kAllocationsGetSize(void) {
    
    return globalAllocBytes;
}

