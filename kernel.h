//
// AVR Micro Kernel

#ifndef ____KERNEL_MAIN__
#define ____KERNEL_MAIN__

#define __CORE_MAIN_          // Include the device driver and kernel module systems
#define __CORE_SCHEDULER_     // Include the task scheduling sub-system

//#define __BOOT_SAFEMODE_      // Load the minimum drivers required to boot to a prompt
//#define __CORE_LIGHTWEIGHT_   // Load minimal modules


//
// Default system device names

const char _DISPLAY_CONSOLE__[]   = "display";
const char _KEYBOARD_INPUT__[]    = "keyboard";
const char _EXTENDED_MEMORY__[]   = "exMem";
const char _DATA_MEMORY__[]       = "nvMem";
const char _COMMAND_CONSOLE__[]   = "console";


// Standard includes
#include "kernel/std/cstring.h"     // C string functions
#include "kernel/string_const.h"    // General purpose string constants
#include "kernel/std/pointers.h"    // Pointer wrapper


// System tables
#include "kernel/drivers.h"         // Device drivers
#include "kernel/modules.h"         // Application modules
#include "kernel/bus.h"             // System bus interface


#include "kernel/scheduler.h"       // Task scheduler


#endif

