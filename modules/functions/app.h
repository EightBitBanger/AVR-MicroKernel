//
// Application test

void __application_main_(void);

struct ModuleLoaderApplication {
	ModuleLoaderApplication() {
		load_module("app",  4, __application_main_);
	}
}static moduleLoaderApplication;

void executable_task(void);

//
// Entry point

void __application_main_(void) {
	
	Device memDriverPtr;
	Device consoleDriverPtr;
	
	// Check driver support
	if (get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memDriverPtr) == 0) {
		console.print(error_exmem_not_installed, sizeof(error_exmem_not_installed));
		console.printLn();
		return;
	}
	
	// Launch the application task
	task_create("app", 4, executable_task, _TASK_PRIORITY_BACKGROUND__, _TASK_USER__);
	
	return;
}





uint8_t app_cntr = 0;

void executable_task(void) {
	
	Bus device_bus;
	
	bus_write_byte(device_bus, 0xc0000, app_cntr);
	
	//bus_write_byte(device_bus, 0xd0000, app_cntr * app_cntr);
	
	
	app_cntr++;
	
	
	
	
	return;
}


