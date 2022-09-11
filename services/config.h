//
// System service configuration

#ifndef __ARDUINO_BOARD_

#include "console/console.h"        // Command console
#include "routing/router.h"         // Network packet router

struct ServiceLoader {
	
	ServiceLoader() {
		
		__scheduler_init_();
		
		task_create("console", sizeof("console"), keyboard_event_handler, TASK_PRIORITY_REALTIME, TASK_TYPE_SERVICE);
		
		//task_create("server", sizeof("server"), router_entry_point, 10, TASK_TYPE_SERVICE);
		
	};
	
} static serviceLoader;




#endif

