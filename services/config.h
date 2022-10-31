//
// System service configuration

#ifndef __ARDUINO_BOARD_

#include "virtual_file_system/vfs.h"      // Virtual system abstraction
#include "console/console.h"              // Command console interface
//#include "routing/router.h"               // Network packet router

struct ServiceLoader {
	
	ServiceLoader() {
		
		__scheduler_init_();
		
		task_create("console", sizeof("console"), keyboard_event_handler, TASK_PRIORITY_REALTIME, TASK_TYPE_SERVICE);
		
		//task_create("server", sizeof("server"), router_entry_point, TASK_PRIORITY_NORMAL, TASK_TYPE_SERVICE);
		
	};
	
} static serviceLoader;




#endif

