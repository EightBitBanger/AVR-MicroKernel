#ifndef __KERNEL_ALLOCATOR_
#define __KERNEL_ALLOCATOR_

#define ALLOCATOR_BLOCK_SIZE  1024

struct Allocator {
    
    char block[ALLOCATOR_BLOCK_SIZE];
    
};


void AllocateExternalMemory(void);


#endif
