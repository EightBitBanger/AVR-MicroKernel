//
// Application testing ground

void application_entry_point(void);
void application_task(void);

#define __MODULE_NAME_  "app"

struct ModuleLoaderMemCheck {
	
	Bus device_bus;
	uint8_t counter;
	
	ModuleLoaderMemCheck() {
		load_module(__MODULE_NAME_,  sizeof(__MODULE_NAME_), application_entry_point);
	}
}static memCheckModuleLoader;
#undef __MODULE_NAME_


void application_entry_point(void) {
	
	memCheckModuleLoader.counter = 0;
	
	memCheckModuleLoader.device_bus.waitstate_write = 0;
	memCheckModuleLoader.device_bus.waitstate_read  = 0;
	
	task_create("app", sizeof("app"), application_task, _TASK_PRIORITY_NORMAL__, _TASK_USER__);
	
	return;
}


void application_task(void) {
	
	bus_write_byte(memCheckModuleLoader.device_bus, 0x60002, memCheckModuleLoader.counter);
	
	memCheckModuleLoader.counter++;
	
	return;
}


