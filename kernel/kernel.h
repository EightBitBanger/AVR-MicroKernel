//
// AVR Micro Kernel

#ifndef __KERNEL_MAIN__
#define __KERNEL_MAIN__

// Default device names
const char _DISPLAY_CONSOLE__[]   = "display";
const char _COMMAND_CONSOLE__[]   = "console";
const char _KEYBOARD_INPUT__[]    = "keyboard";
const char _EXTENDED_MEMORY__[]   = "ExMem";
const char _HARD_STORAGE__[]      = "Storage";


#include "string_const.h"

#define nop  asm("nop")

#include "tables\drivers.h"    // Device drivers
#include "tables\modules.h"    // Application modules
#include "tables\scheduler.h"  // Task scheduler
#include "bus.h"               // System bus interface


// Standard includes
#include "std\pointers.h"
#include "std\cstring.h"

#include "syscalls.h"          // System call wrap functions



#endif

