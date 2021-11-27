//
// Kill command

void command_kill(void) {
	
	char keyboard_string[_TASK_NAME_SIZE__];
	char task_name[_TASK_NAME_SIZE__];
	for (uint8_t a=0; a < _TASK_NAME_SIZE__; a++) {keyboard_string[a] = 0x20; task_name[a] = 0x20;}
	
	// Get keyboard string
	for (uint8_t a=0; a < _TASK_NAME_SIZE__; a++) keyboard_string[a] = console.keyboard_string[a + 5];
	
	// Check running tasks/services
	for (uint8_t i=0; i<_TASK_LIST_SIZE__; i++) {
		
		if (scheduler.taskName[i][0] == 0x20) continue;
		
		// Get task name
		for (uint8_t a=0; a < console.last_string_length-1; a++) task_name[a] = scheduler.taskName[i][a];
		
		// Check function parameter
		if (string_compare(keyboard_string, task_name, console.last_string_length - 5) == 1) {
			
			for (uint8_t c=0; c < _TASK_NAME_SIZE__; c++) scheduler.taskName[i][c] = 0x20;
			scheduler.taskType[i] = 0x00;
			scheduler.taskPriority[i] = 0x00;
			scheduler.taskCounters[i] = 0x00;
			scheduler.task_pointer_table[i] = NULL_f;
			
			console.print(message_task_stopped, sizeof(message_task_stopped));
			console.printLn();
			
			return;
			
		}
		
	}
	
	console.print(message_task_not_found, sizeof(message_task_not_found));
	console.printLn();
	return;
}

