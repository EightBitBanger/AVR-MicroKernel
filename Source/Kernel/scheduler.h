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
		for (index=0; index <= _TASK_LIST_SIZE__; index++) {
			if (task_pointer_table[index] == 0) break; else continue;
		}
		
		// Check no free slots
		if (index == _TASK_LIST_SIZE__) return 0;
		
		// Launch the new task
		taskPriority[index] = priority;
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

/*

// Get a library function address by its device name
EntryPtr& getFuncAddress(const char device_name[], uint8_t name_length) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__) return (EntryPtr&)nullfunction;
	
	// Function look up
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == device_name[a]) count++; else break;
			
			if (count >= name_length)
			return deviceDriverTable.driver_entrypoint_table[i];
			
		}
		
	}
	
	return (EntryPtr&)nullfunction;
}

// Call an external function from a library function pointer
uint8_t callExtern(EntryPtr library_function, uint8_t function_call, uint8_t& paramA=NULL, uint8_t& paramB=NULL, uint8_t& paramC=NULL, uint8_t& paramD=NULL) {
	
	// Check valid pointer
	if (library_function == (EntryPtr&)nullfunction) return 0;
	
	// Find pointer in the driver table
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.driver_entrypoint_table[i] == library_function) {
			library_function(function_call, paramA, paramB, paramC, paramD);
			return 1;
		}
		
	}
	
	return 1;
}


*/


