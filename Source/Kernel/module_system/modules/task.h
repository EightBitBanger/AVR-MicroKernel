//
// Service command

void command_task(void);

struct ModuleServiceCommand {
	
	ModuleServiceCommand() {
		
		const char command_name[] = "task";
		loadModule(&command_task, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleServiceCommand moduleServiceCommand;

void command_task(void) {
	
	// List running tasks/services
	for (uint8_t i=0; i<_TASK_LIST_SIZE__; i++) {
		
		if (scheduler.taskName[i][0] == 0x20) continue;
		
		console.printChar(scheduler.taskType[i]);
		console.printSpace();
		
		for (uint8_t a=0; a < _TASK_NAME_SIZE__; a++) {
			char nameChar = scheduler.taskName[i][a];
			console.printChar(nameChar);
		}
		
		console.printLn();
	}
	
	return;
}

