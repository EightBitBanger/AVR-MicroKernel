//
// Application example

void application_entry_point(void);
void application_task(void);

#define __MODULE_NAME_  "app"

struct ModuleLoaderApplication {
	
	Bus device_bus;
	uint8_t counter;
	
	ModuleLoaderApplication() {
		
		counter = 0;
		
		device_bus.waitstate_write = 0;
		device_bus.waitstate_read  = 0;
		
		load_module(__MODULE_NAME_,  sizeof(__MODULE_NAME_), application_entry_point);
	}
}static applicationModuleLoader;



volatile uint32_t var  = 8;
volatile uint32_t test = 3;
volatile uint32_t res  = 276;

void test_function() {
	
	for (uint8_t i=0; i < 100; i++) {
		
		res += var * test;
		
	}
	
	console.clearBuffer();
	_delay_ms(50);
	
	console.setCursorPosition(0, 0);
	
	console.printInt(res);
	console.printLn();
	
	return;
}


void application_entry_point(void) {
	
	
	// External memory kernel function table call test
	kernel_load_module(0, (void(*)())test_function);
	
	kernel_call_extern(0);
	
	
	task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), application_task, _TASK_PRIORITY_NORMAL__, _TASK_USER__);
	
	
	/*
	
	Device extended_memory;
	Device console_device;
	
	if (get_func_address(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), console_device) == 0) return;
	
	if (get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), extended_memory) == 0) {
		
		for (uint8_t i=0; i<sizeof(error_exmem_not_installed); i++) 
			call_extern(console_device, 0x00, (uint8_t&)error_exmem_not_installed[i]);
		
		call_extern(console_device, 0x01);
		
		return;
	}
	
	WrappedPointer pointer;
	pointer.address = 0x07fff;
	
	call_extern(console_device, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	call_extern(console_device, 0x00);
	*/
	return;
}


void application_task(void) {
	
	
	return;
}

#undef __MODULE_NAME_

