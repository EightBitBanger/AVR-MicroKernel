//
// Task command

void command_task(void);

struct ModuleLoaderTask {
	ModuleLoaderTask() {
		loadModule(command_task, "task", 5);
	}
};
ModuleLoaderTask loadModuleTask;

void command_task(void) {
	
	// List running tasks/services
	for (uint8_t i=0; i<_PROCESS_LIST_SIZE__; i++) {
		
		if (scheduler.taskName[i][0] == 0x20) continue;
		
		// Task type
		console.printChar(scheduler.taskType[i]);
		console.printSpace();
		
		for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++) {
			char nameChar = scheduler.taskName[i][a];
			console.printChar(nameChar);
		}
		
		console.printLn();
	}
	
	return;
}

