//
// AVR Micro Kernel

#ifndef ____KERNEL_MAIN__
#define ____KERNEL_MAIN__


#define __CORE_MAIN_          // Include the device driver and kernel module systems
#define __CORE_SCHEDULER_     // Include the task scheduling sub-system
#define __CORE_BUS_           // Include the bus interface

//#define __BOOT_LIGHTWEIGHT_   // Boot with minimal modules
//#define __BOOT_SAFEMODE_      // Boot with the minimum drivers required to boot to a prompt


#define _32_BIT_POINTERS__



// General purpose string constants
#include "kernel/string_const.h"

// Standard includes
#include "kernel/std/cstring.h"     // C string functions
#include "kernel/std/pointers.h"    // Pointer wrapper


// System tables
#include "kernel/drivers.h"         // Device drivers
#include "kernel/modules.h"         // Application modules
#include "kernel/bus.h"             // System bus interface


#include "kernel/scheduler.h"       // Task scheduler


#endif

