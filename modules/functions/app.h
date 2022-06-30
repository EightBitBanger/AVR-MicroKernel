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
	
	// External memory kernel function table call test
	//kernel_load_module(0, (void(*)())application_task);
	
	//kernel_call_extern(0);
	
	
	
	
	Device extended_memory;
	Device console_device;
	
	if (get_func_address(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), console_device) == 0) return;
	
	if (get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), extended_memory) == 0) return;
	
	WrappedPointer pointer;
	pointer.address = 255;
	
	call_extern(extended_memory, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	call_extern(extended_memory, 0x00);
	
	
	return;
}


void application_task(void) {
	
	task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), application_task, _TASK_PRIORITY_NORMAL__, _TASK_USER__);
	
	return;
}

#undef __MODULE_NAME_

