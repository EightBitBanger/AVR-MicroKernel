//
// Task command -List running tasks

void command_task(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "task"

struct ModuleLoaderTask {
	ModuleLoaderTask() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Device)command_task, _DEVICE_TYPE_MODULE__);
	}
}static loadModuleTask;
#undef __MODULE_NAME_


void command_task(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device consoleDriver;
	if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	
	// List running processes/services
	for (uint8_t i=0; i<_PROCESS_LIST_SIZE__; i++) {
		
		if (proc_info.processName[i][0] == 0x20) continue;
		
		// Process ID
		uint8_t PID = i + 0x30 + 1;
		call_extern(consoleDriver, 0x00, (uint8_t&)PID);
		call_extern(consoleDriver, 0x03); // Space
		
		// Task type
		//call_extern(consoleDriver, 0x00, (uint8_t&)proc_info.processType[i]);
		//call_extern(consoleDriver, 0x03); // Space
		
		for (uint8_t a=0; a < _PROCESS_NAME_LENGTH_MAX__; a++) {
			uint8_t nameChar = proc_info.processName[i][a];
			if (nameChar == 0x20) break;
			call_extern(consoleDriver, 0x00, nameChar);
		}
		
		call_extern(consoleDriver, 0x01); // new line
		
	}
	
	return;
}

