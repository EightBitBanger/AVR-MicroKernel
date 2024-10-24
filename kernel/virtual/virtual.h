#ifndef __VIRTUAL_MEMORY_ACCESS_
#define __VIRTUAL_MEMORY_ACCESS_

#include <kernel/kernel.h>



void VirtualWrite(uint32_t address, uint8_t* byte, uint32_t size);

void VirtualRead(uint32_t address, uint8_t* byte, uint32_t size);

#endif
