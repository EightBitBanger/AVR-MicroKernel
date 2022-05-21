//
// AVR Micro Kernel

#ifndef __KERNEL_MAIN__
#define __KERNEL_MAIN__

// Default device names
const char _DISPLAY_CONSOLE__[]   = "display";
const char _COMMAND_CONSOLE__[]   = "console";
const char _KEYBOARD_INPUT__[]    = "keyboard";
const char _EXTENDED_MEMORY__[]   = "HiMem";

#include "string_const.h"

#define nop  asm("nop")

#include "tables\driver_table.h"  // Device drivers
#include "tables\module_table.h"  // Application modules
#include "bus.h"                  // System bus interface

// Standard includes
#include "std\cstring.h"
#include "std\pointers.h"
//#include "std\strings.h"

#include "syscalls.h"

#include "scheduler.h"

//#include "framework.h"

#endif

