//
// Co-operative scheduler

#ifndef ____TASK_SCHEDULER__
#define ____TASK_SCHEDULER__

#include <avr/interrupt.h>

#define _PROCESS_LIST_SIZE__              10
#define _PROCESS_NAME_LENGTH_MAX__        10

#define _TASK_USER__                      'u' // User task
#define _TASK_SERVICE__                   's' // System service task
#define _TASK_VOLATILE__                  'v' // Volatile task




#define _TASK_PRIORITY_BACKGROUND__       80
#define _TASK_PRIORITY_LOW__              40
#define _TASK_PRIORITY_NORMAL__           20
#define _TASK_PRIORITY_HIGH__             10
#define _TASK_PRIORITY_REALTIME__         1

#ifdef __CORE_SCHEDULER_

// Schedule a new task
uint8_t task_create(const char task_name[], uint8_t name_length, void(*task_ptr)(), uint32_t priority, uint8_t task_type);

// Remove a running task
uint8_t task_destroy(uint8_t PID);

// Get a task index by its name
uint8_t get_task_index(const char task_name[], uint8_t name_length);



// Millisecond timer counter
static volatile uint32_t clock_ms = 0;

//
// Process information descriptor table

struct ProcessDescriptorTable {
	
	uint8_t  processName[_PROCESS_LIST_SIZE__][_PROCESS_NAME_LENGTH_MAX__];
	uint8_t  processType[_PROCESS_LIST_SIZE__];
	uint16_t processPriority[_PROCESS_LIST_SIZE__];
	uint16_t processCounters[_PROCESS_LIST_SIZE__];
	void (*process_pointer_table[_PROCESS_LIST_SIZE__])();
	
}volatile static proc_info;


uint8_t task_create(const char task_name[], uint8_t name_length, void(*task_ptr)(), uint32_t priority, uint8_t task_type) {
	
	// Check name length
	if (name_length > _PROCESS_NAME_LENGTH_MAX__) 
		name_length = _PROCESS_NAME_LENGTH_MAX__;
	
	// Find an available slot
	uint8_t index;
	for (index=0; index < _PROCESS_LIST_SIZE__; index++) {
		if (proc_info.processPriority[index] == 0) break;
	}
	
	// No free slots
	if (index >= _PROCESS_LIST_SIZE__) return 0;
	
	// Launch the new process
	for (uint8_t a=0; a < name_length-1; a++)
		proc_info.processName[index][a] = task_name[a];
	
	proc_info.processType[index]           = task_type;
	proc_info.processPriority[index]       = priority;
	proc_info.processCounters[index]       = 0;
	proc_info.process_pointer_table[index] = task_ptr;
	
	return index + 1;
}


uint8_t task_destroy(uint8_t index) {
	
	uint8_t PID = index - 1;
	
	if (PID > _PROCESS_LIST_SIZE__) return 0;
	if (proc_info.processName[PID][0] == 0x20) return 0;
	
	proc_info.processType[PID]     = 0;
	proc_info.processPriority[PID] = 0;
	proc_info.processCounters[PID] = 0;
	proc_info.process_pointer_table[PID] = 0;
	
	for (uint8_t i=0; i < _PROCESS_NAME_LENGTH_MAX__; i++)
		proc_info.processName[PID][i] = 0x20;
	
	return 1;
}


uint8_t get_task_index(char task_name[], uint8_t name_length) {
	
	// Check name length
	if (name_length > _PROCESS_NAME_LENGTH_MAX__)
		name_length = _PROCESS_NAME_LENGTH_MAX__;
	
	char list_task_name[name_length];
	
	for (uint8_t index=0; index< _PROCESS_LIST_SIZE__; index++) {
		
		if (proc_info.processName[index][0] == 0x20) continue;
		
		// Get task name from the list
		for (uint8_t i=0; i < name_length; i++) 
			list_task_name[i] = proc_info.processName[index][i];
		
		if (string_compare(task_name, list_task_name, name_length) == 1) 
			return (index + 1);
		
	}
	
	return 0;
}



//
// Scheduler timer interrupt
ISR (TIMER0_COMPA_vect) {
#ifdef __CORE_SCHEDULER_
	
	clock_ms++;
	
	for (uint8_t PID=0; PID < _PROCESS_LIST_SIZE__; PID++) {
		
		if (proc_info.processPriority[PID] == 0) continue;
		
		if (proc_info.processCounters[PID] >= proc_info.processPriority[PID]) {
			proc_info.processCounters[PID]=0;
			proc_info.process_pointer_table[PID]();
		} else {
			proc_info.processCounters[PID]++;
			continue;
		}
		
		switch (proc_info.processType[PID]) {
			
			case _TASK_VOLATILE__: {
				
				for (uint8_t i=0; i < _PROCESS_NAME_LENGTH_MAX__; i++) 
					proc_info.processName[PID][i] = 0x20;
				
				proc_info.processType[PID] = 0;
				proc_info.processPriority[PID] = 0;
				proc_info.processCounters[PID] = 0;
				proc_info.process_pointer_table[PID] = 0;
				
				break;
			}
			
			
			default: break;
			
		}
		
		
	}
#endif
	return;
}

#endif


void __scheduler_init_(void) {
	
	for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
		proc_info.processType[i]     = 0x00;
		proc_info.processPriority[i] = 0x00;
		proc_info.processCounters[i] = 0x00;
		proc_info.process_pointer_table[i] = 0;
		
		for (uint8_t a=0; a < _PROCESS_NAME_LENGTH_MAX__; a++)
			proc_info.processName[i][a] = 0x20;
	}
	
}


void __scheduler_start(void) {
	
	//
	// Timer1 - Low level function scheduler
	TCCR0A  = 2;      // 0 - Normal counter and waveform
	                  // 2 - No OC0A, no OC0B, CTC
	
	TCCR0B  = 4;      // 0 - Timer stopped
	                  // 1 - No prescaler
					  // 2 - div8
					  // 3 - div64
					  // 4 - div256
					  // 5 - div1024
	
	TIMSK0  = 2;      // 1 - Timer overflow int
	                  // 2 -  Timer output Compare A Match
	                  // 3 -  Timer output Compare B Match
	
	//TCNT0   = 0;      // (TIMER1_OVF_vect)    Start timer off from 0
	OCR0A   = 93;     // (TIMER0_COMPA_vect)  24M/256/(1+93) = 997 Hz
	
	
	// Enable interrupts
	sei();
	
	return;
}


void __scheduler_stop(void) {
	
	TCCR0B  = 0;
	
	return;
}


#endif

