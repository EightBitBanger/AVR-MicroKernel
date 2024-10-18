#include <avr/io.h>

#include <kernel/syscalls/alloc/new.h>

uint32_t numberOfAllocs = 0;

uint32_t new(uint32_t size) {
    
    uint8_t deviceLetter = fsGetDeviceRoot();
    fsSetDeviceLetter('x');
    
    union Pointer ptr;
    ptr.address = fsAllocate(size);
    
    fsSetDeviceLetter(deviceLetter);
    return ptr.address;
}

void delete(uint32_t ptr) {
    
    
    return;
}

