//
// Memory testing

void mem_check_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
void mem_check_task(void);

#define __MODULE_NAME_  "memchk"

struct ModuleLoaderApp {
	
	Bus device_bus;
	uint8_t byte_counter;
	uint32_t address_counter;
	
	ModuleLoaderApp() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)mem_check_entry_point, _DEVICE_TYPE_MODULE__);
	}
}static loadModuleAppEntryPoint;
#undef __MODULE_NAME_


void mem_check_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	loadModuleAppEntryPoint.byte_counter    = 0;
	loadModuleAppEntryPoint.address_counter = 0x00000;
	
	loadModuleAppEntryPoint.device_bus.waitstate_write = 0;
	loadModuleAppEntryPoint.device_bus.waitstate_read  = 2;
	
	task_create("memchk", sizeof("memchk"), mem_check_task, _TASK_PRIORITY_NORMAL__, _TASK_SERVICE__);
	
	return;
}


void mem_check_task(void) {
	
	char byte;
	
	bus_write_byte(loadModuleAppEntryPoint.device_bus, loadModuleAppEntryPoint.address_counter, loadModuleAppEntryPoint.byte_counter);
	bus_read_byte(loadModuleAppEntryPoint.device_bus, loadModuleAppEntryPoint.address_counter, byte);
	
	if (byte != loadModuleAppEntryPoint.byte_counter) {
		console.print(error_stack_fault, sizeof(error_stack_fault));
		while(1);
	}
	
	loadModuleAppEntryPoint.byte_counter++;
	
	loadModuleAppEntryPoint.address_counter++;
	if (loadModuleAppEntryPoint.address_counter == 0x40000) loadModuleAppEntryPoint.address_counter=0x00000;
	
	return;
}


