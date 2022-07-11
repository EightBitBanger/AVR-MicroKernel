//
// Application example

void application_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
void application_task(void);

#define __MODULE_NAME_  "app"

struct ModuleLoaderApplication {
	
	ModuleLoaderApplication() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)application_entry_point, _DEVICE_TYPE_MODULE__);
	}
}static applicationModuleLoader;



void application_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), application_task, _TASK_PRIORITY_NORMAL__, _TASK_USER__);
	
	return;
}

uint16_t counter = 0;

void application_task(void) {
	
	return;
}

#undef __MODULE_NAME_

