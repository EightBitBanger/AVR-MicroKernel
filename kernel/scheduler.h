//
// Cooperative scheduler
#include<avr/interrupt.h>

#define _PROCESS_LIST_SIZE__  24
#define _PROCESS_NAME_SIZE__  16

#define _PROCESS_TYPE_USER__      0x75 // Process is a user program
#define _PROCESS_TYPE_VOLATILE__  0x76 // Process will terminate after execution
#define _PROCESS_TYPE_SERVICE__   0x73 // Process is a system service routine

struct ProcessScheduler {
	
	uint8_t  processName[_PROCESS_LIST_SIZE__][_PROCESS_NAME_SIZE__];
	uint8_t  processType[_PROCESS_LIST_SIZE__];
	uint16_t processPriority[_PROCESS_LIST_SIZE__];
	uint16_t processCounters[_PROCESS_LIST_SIZE__];
	void (*process_pointer_table[_PROCESS_LIST_SIZE__])();
	
	ProcessScheduler() {
		
		// Initiate the interrupt timer
		TCCR0A  = 0x00;          // Normal counter and waveform
		TCCR0B |= (1<<CS10);     // Timer mode with NO prescaler
		TIMSK0 |= (1 << TOIE0);  // Enable counter overflow interrupt (TOIEn)
		
		// Setup the task list
		for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
			for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++) processName[i][a] = 0x20;
			processType[i]     = 0x00;
			processPriority[i] = 0x00;
			processCounters[i] = 0x00;
			process_pointer_table[i] = (Process&)NULL_f;
		}
		
	}
	
	// Schedule a new process
	uint8_t createProcess(const char name[], uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t task_type) {
		
		if ((priority == 0) || (name_length > _PROCESS_NAME_SIZE__-1)) return 0;
		
		// Find an available slot
		uint8_t index;
		for (index=0; index <= _PROCESS_LIST_SIZE__; index++) {
			if (processPriority[index] == 0) break; else continue;
		}
		
		// No free slots
		if (index == _PROCESS_LIST_SIZE__) return 0;
		
		// Launch the new task
		for (uint8_t a=0; a < name_length-1; a++)
		processName[index][a]    = name[a];
		
		processType[index]           = task_type;
		processPriority[index]       = priority;
		processCounters[index]       = 0;
		process_pointer_table[index] = task_ptr;
		
		return 1;
	}
	
	// Get a process pointer by a process name
	Process& getProcess(const char process_name[], uint8_t name_length) {
		
		if (name_length > _PROCESS_NAME_SIZE__-1) return (Process&)NULL_f;
		
		// Function look up
		for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
			
			if (processName[i][0] == 0x20) continue;
			
			uint8_t count=1;
			for (uint8_t a=0; a < name_length; a++) {
				char nameChar = processName[i][a];
				if (nameChar == process_name[a]) count++; else break;
			}
			
			if (count >= name_length)
			return process_pointer_table[i];
			
		}
		
		return (Process&)NULL_f;
	}
	
	// Stop an active process
	uint8_t killProcess(const char process_name[], uint8_t name_length) {
		
		Process& process = getProcess(process_name, name_length);
		
		// Find the task
		uint8_t index;
		for (index=0; index <= _PROCESS_LIST_SIZE__; index++)
		if (process_pointer_table[index] == process) break; else continue;
		
		// Check no free slots
		if (index == _PROCESS_LIST_SIZE__) return 0;
		
		// Kill the task
		processPriority[index] = 0;
		processCounters[index] = 0;
		process_pointer_table[index] = (Process&)NULL_f;
		
		return 1;
	}
	
	// Start the scheduler with the given timer priority
	void start(void) {
		TCNT0 = 100;  // Enable & set timer
		sei();        // Enable interrupts
	}
	
};
ProcessScheduler scheduler;

// Scheduler interrupt handler
ISR (TIMER0_OVF_vect) {
	
	for (uint8_t index=0; index < _PROCESS_LIST_SIZE__; index++) {
		
		if (scheduler.processPriority[index] == 0) continue;
		
		// Check per process counter
		scheduler.processCounters[index]++;
		if (scheduler.processCounters[index] < scheduler.processPriority[index]) {continue;} else {
			// Run the process
			scheduler.processCounters[index]=0;
			scheduler.process_pointer_table[index]();
		}
		
		// Check volatility
		if (scheduler.processType[index] == _PROCESS_TYPE_VOLATILE__) {
			// Remove process
			for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++) scheduler.processName[index][a] = 0x20;
			scheduler.processType[index] = 0;
			scheduler.processPriority[index] = 0;
			scheduler.processCounters[index] = 0;
			scheduler.process_pointer_table[index] = (Process&)NULL_f;
		}
		
	}
	
	return;
}


