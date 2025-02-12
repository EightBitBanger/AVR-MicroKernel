#ifndef __SYSCALL_GRAPHICS_API_
#define __SYSCALL_GRAPHICS_API_

#include <kernel/kernel.h>



void swapBuffers(void);

int8_t glInit(void);

void glClear(uint8_t character);

#endif
