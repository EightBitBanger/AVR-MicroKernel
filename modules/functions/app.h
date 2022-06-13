//
// Application testing ground

void application_entry_point(void);
void application_task(void);

#define __MODULE_NAME_  "app"

struct ModuleLoaderApp {
	ModuleLoaderApp() {
		load_module(__MODULE_NAME_,  sizeof(__MODULE_NAME_), application_entry_point);
	}
}static loadModuleAppEntryPoint;
#undef __MODULE_NAME_

Bus device_bus;
uint8_t counter;

void application_entry_point(void) {
	
	counter = 0;
	
	device_bus.waitstate_write = 0;
	device_bus.waitstate_read  = 0;
	
	task_create("app", sizeof("app"), application_task, _TASK_PRIORITY_HIGH__, _TASK_USER__);
	
	return;
}


void application_task(void) {
	
	bus_write_byte(device_bus, 0x60002, counter);
	
	counter++;
	
	return;
}


