#include <avr/io.h>

#include <kernel/syscalls/alloc/new.h>

uint32_t globalAllocs = 0;
uint32_t globalAllocBytes = 0;

uint32_t __new__(uint32_t size) {
    
    if (size == 0) 
        return 0;
    
    uint8_t deviceLetter = fsGetDeviceRoot();
    fsSetDeviceLetter('x');
    
    union Pointer ptr;
    ptr.address = fsAllocate(size);
    
    if (ptr.address != 0) {
        
        globalAllocs++;
        globalAllocBytes += size;
        
    }
    
    fsSetDeviceLetter(deviceLetter);
    return ptr.address;
}

void __delete__(uint32_t ptr) {
    
    if (ptr == 0) 
        return;
    
    uint8_t deviceLetter = fsGetDeviceRoot();
    fsSetDeviceLetter('x');
    
    // Get allocation size
    union Pointer allocSize;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(ptr + FILE_OFFSET_SIZE, &allocSize.byte_t[i]);
    
    if (fsFree( ptr ) != 0) {
        
        globalAllocs--;
        globalAllocBytes -= allocSize.address;
        
    }
    
    fsSetDeviceLetter(deviceLetter);
    return;
}

