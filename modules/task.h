//
// Task command -List running tasks

void command_task(void);

struct ModuleLoaderTask {
	ModuleLoaderTask() {
		loadModule(command_task, "task", 5);
	}
}static loadModuleTask;

void command_task(void) {
	
	DriverEntryPoint consoleDriver;
	if (getFuncAddress(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	
	// List running processes/services
	for (uint8_t i=0; i<_PROCESS_LIST_SIZE__; i++) {
		
		if (proc_info.processName[i][0] == 0x20) continue;
		
		// Task type
		callExtern(consoleDriver, 0x00, (uint8_t&)proc_info.processType[i]);
		callExtern(consoleDriver, 0x03); // Space
		
		for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++) {
			uint8_t nameChar = proc_info.processName[i][a];
			callExtern(consoleDriver, 0x00, nameChar);
		}
		
		callExtern(consoleDriver, 0x01); // new line
		
	}
	
	return;
}

