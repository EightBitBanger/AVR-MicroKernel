//
// AVR Micro Kernel

#ifndef ____KERNEL_MAIN__
#define ____KERNEL_MAIN__

#define __CORE_MAIN_
#define __CORE_SCHEDULER_

//#define __CORE_LIGHTWEIGHT_
//#define __BOOT_SAFEMODE_

#ifndef __CORE_LIGHTWEIGHT_
  #include "kernel/string_const.h"    // General purpose string constants
  
  // Standard includes
  #include "kernel/std/pointers.h"    // Pointer wrapper
  #include "kernel/std/cstring.h"     // C string functions
#endif

// System tables
#ifdef __CORE_MAIN_
  #include "kernel/bus.h"             // System bus interface
  #include "kernel/drivers.h"         // Device drivers
  #include "kernel/modules.h"         // Application modules
#endif

#ifdef __CORE_SCHEDULER_
  #include "kernel/scheduler.h"       // Task scheduler
#endif


#endif

