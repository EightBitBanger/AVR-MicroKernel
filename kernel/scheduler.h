//
// Cooperative scheduler
#include<avr/interrupt.h>

#define _PROCESS_LIST_SIZE__  24
#define _PROCESS_NAME_SIZE__  16

#define _PROCESS_TYPE_USER__      0x75 // Process is a user program
#define _PROCESS_TYPE_VOLATILE__  0x76 // Process will terminate after execution
#define _PROCESS_TYPE_SERVICE__   0x73 // Process is a system service routine

typedef void(*Process)();


struct ProcessScheduler {
	
	uint8_t  processName[_PROCESS_LIST_SIZE__][_PROCESS_NAME_SIZE__];
	uint8_t  processType[_PROCESS_LIST_SIZE__];
	uint16_t processPriority[_PROCESS_LIST_SIZE__];
	uint16_t processCounters[_PROCESS_LIST_SIZE__];
	void (*process_pointer_table[_PROCESS_LIST_SIZE__])();
	
	ProcessScheduler() {
		
		for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
			for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++) processName[i][a] = 0x20;
			processType[i]     = 0x00;
			processPriority[i] = 0x00;
			processCounters[i] = 0x00;
			process_pointer_table[i] = 0;
		}
		
	}
	
	// Start the scheduler with the given timer priority
	void start(uint8_t timer_priority) {
		
		TCCR0A  = 0x00;          // Normal counter and waveform
		TCCR0B |= (1 << CS10);   // Timer mode with NO prescaler
		TIMSK0 |= (1 << TOIE0);  // Enable counter overflow interrupt (TOIEn)
		
		TCNT0 = timer_priority;
		sei();
	}
	
	// Stop the scheduler
	void stop(void) {
		
		TCCR0A  = 0x00;
		TCCR0B &= ~(1 << CS10);
		TIMSK0 &= ~(1 << TOIE0);
		
		TCNT0 = 0;
	}
	
	// Schedule a new process
	uint8_t createProcess(const char name[], uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t task_type) {
		
		if ((priority == 0) || (name_length > _PROCESS_NAME_SIZE__-1)) return 0;
		
		// Find an available slot
		uint8_t index;
		for (index=0; index <= _PROCESS_LIST_SIZE__; index++) 
			if (processPriority[index] == 0) break; else continue;
		
		// No free slots
		if (index == _PROCESS_LIST_SIZE__) return 0;
		
		// Launch the new process
		for (uint8_t a=0; a < name_length-1; a++)
			processName[index][a] = name[a];
		
		processType[index]           = task_type;
		processPriority[index]       = priority;
		processCounters[index]       = 0;
		process_pointer_table[index] = task_ptr;
		
		return 1;
	}
	
	// Get a process index by its process name
	uint8_t getProcessIndex(const char process_name[], uint8_t name_length) {
		
		if (name_length > _PROCESS_NAME_SIZE__-1) return 0;
		
		// Function look up
		for (uint8_t index=0; index< _PROCESS_LIST_SIZE__; index++) {
			
			if (processName[index][0] == 0x20) continue;
			
			uint8_t count=1;
			for (uint8_t a=0; a < name_length; a++) {
				char nameChar = processName[index][a];
				if (nameChar == process_name[a]) count++; else break;
			}
			
			// Return the index
			return index;
		}
		
		return 0;
	}
	
	// Stop a running process
	uint8_t killProcess(const char process_name[], uint8_t name_length) {
		
		// Find the task
		uint8_t index = getProcessIndex(process_name, name_length);
		
		// Kill the task
		processType[index]     = 0;
		processPriority[index] = 0;
		processCounters[index] = 0;
		process_pointer_table[index] = 0;
		
		for (uint8_t i=0; i < _PROCESS_NAME_SIZE__; i++) processName[index][i] = 0x20;
		
		return 1;
	}
	
};
ProcessScheduler scheduler;

// Interrupt handler
ISR (TIMER0_OVF_vect) {
	
	for (uint8_t index=0; index < _PROCESS_LIST_SIZE__; index++) {
		
		// Check per process counter
		scheduler.processCounters[index]++;
		if ((scheduler.processCounters[index] < scheduler.processPriority[index]) || (scheduler.processPriority[index] == 0)) {continue;} else {
			// Run the process
			scheduler.processCounters[index]=0;
			scheduler.process_pointer_table[index]();
		}
		
		switch (scheduler.processType[index]) {
			
			case _PROCESS_TYPE_VOLATILE__: {
				// Remove process
				for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++) scheduler.processName[index][a] = 0x20;
				scheduler.processType[index] = 0;
				scheduler.processPriority[index] = 0;
				scheduler.processCounters[index] = 0;
				scheduler.process_pointer_table[index] = 0;
				break;
			}
			
			
			default: break;
			
		}
		
		
	}
	
	return;
}


