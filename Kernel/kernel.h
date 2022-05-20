//
// AVR Micro Kernel

#ifndef __KERNEL_MAIN__
#define __KERNEL_MAIN__

// Default device names
const char _DISPLAY_CONSOLE__[]   = "display";
const char _COMMAND_CONSOLE__[]   = "console";
const char _KEYBOARD_INPUT__[]    = "keyboard";

#define nop  asm("nop")

#include "std\pointers.h"
#include "std\cstring.h"
//#include "std\strings.h"
#include "string_const.h"

#include "bus.h"

#include "tables\driver_table.h"
#include "tables\module_table.h"

#include "scheduler.h"

#include "framework.h"

#endif

