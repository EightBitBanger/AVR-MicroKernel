//
// List command

void command_list(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
void list_function_task(void);

#define __MODULE_NAME_  "list"

struct ModuleLoaderList {
	
	Bus deviceBus;
	
	uint8_t  byte;
	uint32_t iteration;
	uint32_t iterationMax;
	
	ModuleLoaderList() {
		
		deviceBus.waitstate_write = 4;
		deviceBus.waitstate_read  = 4;
		
		byte         = 0x0a;
		iteration    = 0x40000;
		iterationMax = 0x40000 + 80;
		
		load_module(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)command_list, _DEVICE_TYPE_MODULE__);
	}
}static moduleLoaderList;
#undef __MODULE_NAME_


void command_list(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	for (uint8_t i=0; i < 80; i++) {
		
		moduleLoaderList.byte += 1;
		if (moduleLoaderList.byte > 0xfe) moduleLoaderList.byte = 0x00;
		
		//console.printChar(moduleLoaderList.byte);
		
		bus_write_byte(moduleLoaderList.deviceBus, 0xc0000, moduleLoaderList.byte * 8);
		_delay_ms(5);
		
		moduleLoaderList.iteration++;
		if (moduleLoaderList.iteration > moduleLoaderList.iterationMax) moduleLoaderList.iteration=0x40000;
		
	}
	
	//task_create("list", 5, list_function_task, _PRIORITY_NORMAL__, _TASK_VOLATILE__);
	
	return;
}

void list_function_task(void) {
	
}


