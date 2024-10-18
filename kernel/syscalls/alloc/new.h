#ifndef __SYSCALL_NEW_
#define __SYSCALL_NEW_

#include <kernel/kernel.h>

uint32_t new(uint32_t size);

void delete(uint32_t ptr);

#endif
