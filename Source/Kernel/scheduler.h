//
// Task scheduling system

#define _TASK_LIST_SIZE__  16

typedef void(*TaskPtr)();

struct TaskSchedulerSystem {
	
	uint16_t taskPriority[_TASK_LIST_SIZE__];
	uint16_t taskCounters[_TASK_LIST_SIZE__];
	void (*task_pointer_table[_TASK_LIST_SIZE__])(void);
	
	TaskSchedulerSystem() {
		for (uint8_t i=0; i < _TASK_LIST_SIZE__; i++) {
			taskPriority[i] = 0x00;
			taskCounters[i] = 0x00;
			task_pointer_table[i] = 0;
		}
	}
	
	// Process running tasks
	void processTasks(void) {
		
		for (uint16_t index=0; index < _TASK_LIST_SIZE__; index++) {
			
			// Check if the task is registered
			if (taskPriority[index] == 0) continue;
			
			// Increment the task counter
			taskCounters[index]++;
			
			// Check if the task should be executed
			if (taskCounters[index] == taskPriority[index]) {taskCounters[index] = 0;
				
				task_pointer_table[index]();
				
			}
			
		}
		
	}
	
	// Schedule a new task
	uint8_t createTask(void(*task_ptr)(), uint16_t priority) {
		
		if (priority == 0) return 0;
		
		// Find an available slot
		uint8_t index;
		for (index=0; index <= _TASK_LIST_SIZE__; index++) 
			if (task_pointer_table[index] == 0) {break;} else {continue;}
		
		// Check no free slots
		if (index == _TASK_LIST_SIZE__) return 0;
		
		// Launch the new task
		taskPriority[index] = priority * 1000;
		taskCounters[index] = 0;
		task_pointer_table[index] = task_ptr;
		
		return 1;
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


