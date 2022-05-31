//
// AVR kernel core

#ifndef ____KERNEL_MAIN__
#define ____KERNEL_MAIN__

#include "kernel/config.h"

// Standard includes
#include "kernel/std/string_const.h"  // System message strings
#include "kernel/std/cstring.h"       // C string functions
#include "kernel/std/pointers.h"      // Pointer wrapper

// Kernel function tables
#include "kernel/driver_table.h"      // Device drivers
#include "kernel/module_table.h"      // Function modules
#include "kernel/bus.h"               // System bus interface
#include "kernel/scheduler.h"         // Task scheduler

// Kernel tables initiator
struct __INITIATE_KERNEL_TABLES_ {
	__INITIATE_KERNEL_TABLES_() {
		
		__extern_initiate();  // Initiate device driver system
		
		__module_init_();     // Initiate function module system
		
		__scheduler_init_();  // Initiate scheduler
		
	}
} static __initiate_kernel_tables_;

// Load device drivers, function modules and system services
#include "drivers/config.h"
#include "modules/config.h"
#include "services/config.h"

#endif

