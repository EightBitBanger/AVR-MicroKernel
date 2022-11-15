//
// Task command -List running tasks

char msg_task_stopped[]    = "Task stopped";
char msg_task_not_found[]  = "Task not found";

void command_task(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device consoleDriver;
	consoleDriver = (Device)get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__));
	if (consoleDriver == 0) return;
	
	uint8_t page_counter=0;
	
	// Get the parameter from the keyboard string
	uint8_t param0  = console.keyboard_string[sizeof("task")];
	uint8_t param1  = console.keyboard_string[sizeof("task") + 2];
	
	// List running processes/services
	if (param0 == 'l') {
		
		for (uint8_t i=0; i<PROCESS_LIST_SIZE; i++) {
			
			if (proc_info.name[i][0] == 0x20) continue;
			
			// Process ID
			uint8_t PID = i + 0x30 + 1;
			
			console.printChar((uint8_t&)PID);
			console.printSpace();
			
			// Task type
			if (param1 == 't') {
				console.printChar((uint8_t&)proc_info.type[i]);
				console.printSpace();
			}
			
			// Task priority
			if (param1 == 'p') {
				console.printInt((uint8_t&)proc_info.priority[i]);
				console.printSpace();
			}
			
			for (uint8_t a=0; a < PROCESS_NAME_LENGTH_MAX; a++) {
				uint8_t nameChar = proc_info.name[i][a];
				if (nameChar == 0x20) break;
				console.printChar(nameChar);
			}
			
			page_counter++;
			console.printLn();
			
			// Page pause
			if (page_counter > 2) {page_counter = 0;
				call_extern(consoleDriver, 0x0d);
			}
			
		}
		
		return;
	}
	
	// Kill a running task
	if (param0 == 'k') {
		
		uint8_t task_index = param1 - '0';
		if ((task_index >= 0) & (task_index <= 9)) {
			
			if (task_destroy(task_index) == 0) {// Task not stopped
				console.print(msg_task_not_found, sizeof(msg_task_not_found));
			} else {// Task stopped
				console.print(msg_task_stopped, sizeof(msg_task_stopped));
			}
			
		}
		
		console.printLn();
		return;
	}
	
	//
	// No parameters - help anyone ?
	if (param0 == 0x20) {
		
		char help_line_a[] = "task l - list";
		char help_line_b[] = "task k n - kill";
		
		console.print(help_line_a, sizeof(help_line_a)); console.printLn();
		console.print(help_line_b, sizeof(help_line_b)); console.printLn();
		
		return;
	}
	return;
}

