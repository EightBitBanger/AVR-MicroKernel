//
// Kernel configuration

#define __CORE_MAIN_             // Include the device driver and kernel module systems
#define __CORE_SCHEDULER_        // Include the task scheduler
#define __HARDWARE_BUS_          // Include the hardware level bus interface
#define __HARDWARE_AUTO_DETECT_  // Include hardware auto detection

#define __BOOT_SAFEMODE_      // Load only the drivers required to boot
#define __BOOT_LIGHTWEIGHT_   // Load minimal module function required to boot
#define __BOOT_NOSERVICES_    // Load only the services required to boot

#define _32_BIT_POINTERS__
//#define _64_BIT_POINTERS__

