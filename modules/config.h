//
// Kernel function modules

#include "fs/dir.h"             // List files on device
#include "fs/mk.h"              // Make a new file
#include "fs/rm.h"              // Remove a file
#include "fs/rn.h"              // Rename a file
#include "fs/cd.h"              // Change device
#include "fs/copy.h"            // Copy a file to new file
#include "fs/fdisk.h"           // Device format
#include "fs/attr.h"            // File attributes

#include "system/cls.h"         // Clear the screen
#include "system/list.h"        // List raw bytes
#include "system/task.h"        // Task management
#include "system/port.h"        // GPIO port configuration
#include "system/device.h"      // Device table configuration
#include "system/mem.h"         // Memory check

#include "applications/app.h"   // Application test bed
#include "applications/asm.h"   // Assembly tool
#include "applications/net.h"   // Network testing and configuration


struct __ModuleLoader__ {
	
	__ModuleLoader__() {
		
#ifndef __ARDUINO_BOARD_
		
#ifndef __BOOT_LIGHTWEIGHT_
		
#ifdef __BOOT_FS_SUPPORT_
		load_device("dir", sizeof("dir"), (Module)command_dir, DEVICE_TYPE_MODULE);
		load_device("mk", sizeof("mk"), (Module)command_mk, DEVICE_TYPE_MODULE);
		load_device("rm", sizeof("rm"), (Module)command_rm, DEVICE_TYPE_MODULE);
		load_device("rn", sizeof("rn"), (Module)command_rn, DEVICE_TYPE_MODULE);
		load_device("cd", sizeof("cd"), (Module)command_cd, DEVICE_TYPE_MODULE);
		load_device("copy", sizeof("copy"), (Module)command_copy, DEVICE_TYPE_MODULE);
		load_device("asm", sizeof("asm"), (Module)command_asm, DEVICE_TYPE_MODULE);
		load_device("fdisk", sizeof("fdisk"), (Module)command_fdisk, DEVICE_TYPE_MODULE);
		load_device("attr", sizeof("attr"), (Module)command_attrib, DEVICE_TYPE_MODULE);
		
#endif
		
		load_device("cls", sizeof("cls"), (Module)command_clear_screen, DEVICE_TYPE_MODULE);
		load_device("list", sizeof("list"), (Module)command_list, DEVICE_TYPE_MODULE);
		load_device("task", sizeof("task"), (Module)command_task, DEVICE_TYPE_MODULE);
		load_device("mem",  sizeof("mem"), (Module)command_memory, DEVICE_TYPE_MODULE);
		
		//load_device("dev", sizeof("dev"), (Module)command_device, DEVICE_TYPE_MODULE);
		//load_device("port", sizeof("port"), (Module)__port_control_, DEVICE_TYPE_MODULE);
		
		
#ifdef __BOOT_NETWORK_SUPPORT_
		load_device("net",  sizeof("net"), (Module)net_entry_point, DEVICE_TYPE_MODULE);
#endif
		
		// Misc programs
		
		//load_device("app",  sizeof("app"), (Module)application_entry_point, DEVICE_TYPE_MODULE);
		
		
#endif
		
#endif
		
	}
	
} static __moduleLoader__;


