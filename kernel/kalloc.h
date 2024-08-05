#ifndef __KERNEL_ALLOCATOR_
#define __KERNEL_ALLOCATOR_

#include <avr/io.h>

#include <kernel/kernel.h>

uint32_t kAllocGetTotal(void);

void AllocateExternalMemory(void);


#endif
