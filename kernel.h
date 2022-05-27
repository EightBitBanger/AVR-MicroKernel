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
//#define _64_BIT_POINTERS__




// Standard includes
#include "kernel/std/string_const.h"  // System message strings
#include "kernel/std/cstring.h"       // C string functions
#include "kernel/std/pointers.h"      // Pointer wrapper


// System tables
#include "kernel/drivers.h"           // Device drivers
#include "kernel/modules.h"           // Application modules
#include "kernel/bus.h"               // System bus interface

#include "kernel/scheduler.h"         // Task scheduler

// Initiate kernel tables
struct __INITIATE_KERNEL_TABLES_ {
	__INITIATE_KERNEL_TABLES_() {
		
		__extern_initiate();
		
		__module_init_();
		
		__scheduler_init_();
		
	}
} static __initiate_kernel_tables_;


#endif

