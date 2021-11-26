//
// Task scheduling system

#define _TASK_LIST_SIZE__  16
#define _TASK_NAME_SIZE__  10

typedef void(*TaskPtr)();

struct TaskSchedulerSystem {
	
	uint8_t taskName[_TASK_LIST_SIZE__][_TASK_NAME_SIZE__];
	uint16_t taskPriority[_TASK_LIST_SIZE__];
	uint16_t taskCounters[_TASK_LIST_SIZE__];
	void (*task_pointer_table[_TASK_LIST_SIZE__])(void);
	
	TaskSchedulerSystem() {
		for (uint8_t i=0; i < _TASK_LIST_SIZE__; i++) {
			for (uint8_t a=0; a < _TASK_NAME_SIZE__; a++) taskName[i][a] = 0x20;
			taskPriority[i] = 0x00;
			taskCounters[i] = 0x00;
			task_pointer_table[i] = 0;
		}
	}
	
	// Schedule a new task
	uint8_t createTask(const char name[], uint8_t name_length, void(*task_ptr)(), uint16_t priority) {
		
		if ((priority == 0) || (name_length > _TASK_NAME_SIZE__-1)) return 0;
		
		// Find an available slot
		uint8_t index;
		for (index=0; index <= _TASK_LIST_SIZE__; index++) 
			if (task_pointer_table[index] == 0) {break;} else {continue;}
		
		// Check no free slots
		if (index == _TASK_LIST_SIZE__) return 0;
		
		// Launch the new task
		for (uint8_t a=0; a < name_length; a++) 
			taskName[index][a] = name[a];
		taskName[index][name_length-1] = 0x20;
		
		taskPriority[index] = priority;
		taskCounters[index] = 0;
		task_pointer_table[index] = task_ptr;
		
		return 1;
	}
	
	// Get a library function address by its device name
	TaskPtr& findTask(const char task_name[], uint8_t name_length) {
		
		if (name_length > _DRIVER_TABLE_NAME_SIZE__) return (TaskPtr&)NULL_f;
		
		// Function look up
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			
			if (taskPriority[i] == 0) continue;
			
			uint8_t count=1;
			for (uint8_t a=0; a < name_length; a++) {
				
				char nameChar = taskName[i][a];
				if (nameChar == task_name[a]) count++; else break;
				
				if (count >= name_length)
					return task_pointer_table[i];
				
			}
			
		}
		
		return (TaskPtr&)NULL_f;
	}
	
	// Schedule a new task
	uint8_t killTask(void(*task_ptr)()) {
		
		// Find the task
		uint8_t index;
		for (index=0; index <= _TASK_LIST_SIZE__; index++) {
			if (task_pointer_table[index] == task_ptr) break; else continue;
		}
		
		// Check no free slots
		if (index == _TASK_LIST_SIZE__) return 0;
		
		// Kill the task
		taskPriority[index] = 0;
		taskCounters[index] = 0;
		task_pointer_table[index] = 0;
		
		return 1;
	}
	
};
TaskSchedulerSystem scheduler;


