//
// Counter program

void counter_entry_point(void);
void counter_task(void);

#define __MODULE_NAME_  "count"

struct ModuleLoaderCounter {
	
	Bus device_bus;
	uint8_t counter;
	
	ModuleLoaderCounter() {
		load_module(__MODULE_NAME_,  sizeof(__MODULE_NAME_), counter_entry_point);
	}
}static counterModuleLoader;


void counter_entry_point(void) {
	
	counterModuleLoader.counter = 0;
	
	counterModuleLoader.device_bus.waitstate_write = 0;
	counterModuleLoader.device_bus.waitstate_read  = 0;
	
	task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), counter_task, _TASK_PRIORITY_BACKGROUND__ * 10, _TASK_USER__);
	
	return;
}


void counter_task(void) {
	
	bus_write_byte(counterModuleLoader.device_bus, 0x60003, counterModuleLoader.counter);
	
	//bus_write_byte(counterModuleLoader.device_bus, 0x60003, counterModuleLoader.counter);
	
	counterModuleLoader.counter++;
	
	return;
}

#undef __MODULE_NAME_

