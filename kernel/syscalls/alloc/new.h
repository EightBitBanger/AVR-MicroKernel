#ifndef __SYSCALL_NEW_
#define __SYSCALL_NEW_

#include <kernel/kernel.h>

uint32_t __new__(uint32_t size);

void __delete__(uint32_t ptr);


#define new(sz)  __new__(sz)

#define delete(ptr)  __delete__(ptr)

#endif
