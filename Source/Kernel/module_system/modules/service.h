//
// Service command

void command_service(void);

struct ModuleServiceCommand {
	
	ModuleServiceCommand() {
		
		const char command_name[] = "service";
		loadModule(&command_service, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleServiceCommand moduleServiceCommand;

void command_service(void) {
	
	// List running services
	for (uint8_t i=0; i<_TASK_LIST_SIZE__; i++) {
		
		if (scheduler.taskName[i][0] == 0x20) continue;
		
		for (uint8_t a=0; a < _TASK_NAME_SIZE__; a++) {
			
			char nameChar = scheduler.taskName[i][a];
			
			console.printChar(nameChar);
		}
		
		console.printLn();
	}
	
	return;
}

