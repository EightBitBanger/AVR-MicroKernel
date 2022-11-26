//
// Applications modules

#include "system/cls.h"            // Clear the screen
#include "system/list.h"           // List raw bytes
//#include "system/task.h"           // Task management
#include "system/port.h"           // GPIO port configuration
#include "system/mem.h"            // Memory check
#include "system/asm.h"            // Assembly tool

#include "console/console.h"       // Command console service

#include "user/app.h"              // Application testing

struct __ModuleLoader__ {
	
	__ModuleLoader__() {
		
		__scheduler_init_(); // Initiate the scheduler
		
#ifndef __BOOT_LIGHTWEIGHT_
		
#ifdef __BOOT_SYSTEM_FUNCTIONS_
		
		load_device("cls", sizeof("cls"), (Module)command_clear_screen, DEVICE_TYPE_MODULE);
		load_device("list", sizeof("list"), (Module)command_list, DEVICE_TYPE_MODULE);
		//load_device("task", sizeof("task"), (Module)command_task, DEVICE_TYPE_MODULE);
		load_device("mem",  sizeof("mem"), (Module)command_memory, DEVICE_TYPE_MODULE);
		load_device("port", sizeof("port"), (Module)__port_control_, DEVICE_TYPE_MODULE);
		
		load_device("asm", sizeof("asm"), (Module)command_asm, DEVICE_TYPE_MODULE);
		load_device("app", sizeof("app"), (Module)application_entry_point, DEVICE_TYPE_MODULE);
		
		// Launch the command console
		task_create("console", sizeof("console"), keyboard_event_handler, TASK_PRIORITY_REALTIME, TASK_TYPE_SERVICE);
		
		
#endif
		
#endif
		
	}
	
} static __moduleLoader__;


