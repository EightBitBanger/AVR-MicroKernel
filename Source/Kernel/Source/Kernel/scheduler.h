//
// Cooperative task scheduler

#define _PROCESS_LIST_SIZE__  24
#define _PROCESS_NAME_SIZE__  16

#define _PROCESS_TYPE_USER__      0x75 // Task is a user program
#define _PROCESS_TYPE_VOLATILE__  0x76 // Task will terminate after execution
#define _PROCESS_TYPE_SERVICE__   0x73 // Task is a system service routine

struct ProcessScheduler {
	
	uint8_t  taskName[_PROCESS_LIST_SIZE__][_PROCESS_NAME_SIZE__];
	uint8_t  taskType[_PROCESS_LIST_SIZE__];
	uint16_t taskPriority[_PROCESS_LIST_SIZE__];
	uint16_t taskCounters[_PROCESS_LIST_SIZE__];
	void (*task_pointer_table[_PROCESS_LIST_SIZE__])();
	
	ProcessScheduler() {
		for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
			for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++) taskName[i][a] = 0x20;
			taskType[i]     = 0x00;
			taskPriority[i] = 0x00;
			taskCounters[i] = 0x00;
			task_pointer_table[i] = (Process&)NULL_f;
		}
	}
	
	// Schedule a new process.
	uint8_t createProcess(const char name[], uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t task_type) {
		
		if ((priority == 0) || (name_length > _PROCESS_NAME_SIZE__-1)) return 0;
		
		// Find an available slot
		uint8_t index;
		for (index=0; index <= _PROCESS_LIST_SIZE__; index++) {
			if (taskPriority[index] == 0) break; else continue;
		}
		
		// No free slots
		if (index == _PROCESS_LIST_SIZE__) return 0;
		
		// Launch the new task
		for (uint8_t a=0; a < name_length-1; a++) 
			taskName[index][a]    = name[a];
		
		taskType[index]           = task_type;
		taskPriority[index]       = priority;
		taskCounters[index]       = 0;
		task_pointer_table[index] = task_ptr;
		
		return 1;
	}
	
	// Get task function pointer by a process name.
	Process& getProcessTask(const char process_name[], uint8_t name_length) {
		
		if (name_length > _PROCESS_NAME_SIZE__-1) return (Process&)NULL_f;
		
		// Function look up
		for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
			
			if (taskName[i][0] == 0x20) continue;
			
			uint8_t count=1;
			for (uint8_t a=0; a < name_length; a++) {
				char nameChar = taskName[i][a];
				if (nameChar == process_name[a]) count++; else break;
			}
			
			if (count >= name_length)
				return task_pointer_table[i];
			
		}
		
		return (Process&)NULL_f;
	}
	
	// Stop an active process
	uint8_t killTask(const char process_name[], uint8_t name_length) {
		
		Process& process = getProcessTask(process_name, name_length);
		
		// Find the task
		uint8_t index;
		for (index=0; index <= _PROCESS_LIST_SIZE__; index++) 
			if (task_pointer_table[index] == process) break; else continue;
		
		// Check no free slots
		if (index == _PROCESS_LIST_SIZE__) return 0;
		
		// Kill the task
		taskPriority[index] = 0;
		taskCounters[index] = 0;
		task_pointer_table[index] = (Process&)NULL_f;
		
		return 1;
	}
	
};
ProcessScheduler scheduler;


