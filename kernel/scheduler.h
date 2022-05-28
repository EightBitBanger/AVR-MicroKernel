//
// Co-operative scheduler

#ifndef ____TASK_SCHEDULER__
#define ____TASK_SCHEDULER__

#include <util/delay.h>

#include <avr/interrupt.h>

#define _PROCESS_LIST_SIZE__  30
#define _PROCESS_NAME_SIZE__  10

#define _TASK_USER__        'u'
#define _TASK_SERVICE__     's'
#define _TASK_VOLATILE__    'v'

#ifdef __CORE_SCHEDULER_

typedef void(*Task)();


// Schedule a new task
uint8_t task_create(const char name[], uint8_t name_length, void(*task_ptr)(), uint32_t priority, uint8_t task_type);
// Remove a running task
uint8_t task_destroy(const char name[], uint8_t name_length);
// Get a task index by its name
uint8_t get_task_index(const char process_name[], uint8_t name_length);

// Start a running process
uint8_t start_task(const char process_name[], uint8_t name_length);
// Stop a running process
uint8_t stop_task(const char process_name[], uint8_t name_length);

void __scheduler_init_(void);
void __scheduler_start(uint8_t timer_priority);
void __scheduler_stop(void);


//
// Process information descriptor table

struct ProcessDescriptorTable {
	
	uint8_t  processName[_PROCESS_LIST_SIZE__][_PROCESS_NAME_SIZE__];
	uint8_t  processType[_PROCESS_LIST_SIZE__];
	uint16_t processPriority[_PROCESS_LIST_SIZE__];
	uint16_t processCounters[_PROCESS_LIST_SIZE__];
	void (*process_pointer_table[_PROCESS_LIST_SIZE__])();
	
}volatile static proc_info;


//
// Schedule a new task
uint8_t task_create(const char name[], uint8_t name_length, void(*task_ptr)(), uint32_t priority, uint8_t task_type) {
	
	if ((priority == 0) || (name_length > _PROCESS_NAME_SIZE__-1)) return 0;
	
	uint8_t index;
	
	// Find an available slot
	for (index=0; index <= _PROCESS_LIST_SIZE__; index++)
	if (proc_info.processPriority[index] == 0) break;
	
	// No free slots
	if (index == _PROCESS_LIST_SIZE__) return 0;
	
	// Launch the new process
	for (uint8_t a=0; a < name_length-1; a++)
	proc_info.processName[index][a] = name[a];
	
	proc_info.processType[index]           = task_type;
	proc_info.processPriority[index]       = priority;
	proc_info.processCounters[index]       = 0;
	proc_info.process_pointer_table[index] = task_ptr;
	
	return 1;
}


//
// Remove a running task
uint8_t task_destroy(const char process_name[], uint8_t name_length) {
	
	uint8_t PID = get_task_index(process_name, name_length);
	
	proc_info.processType[PID]     = 0;
	proc_info.processPriority[PID] = 0;
	proc_info.processCounters[PID] = 0;
	proc_info.process_pointer_table[PID] = 0;
	
	for (uint8_t i=0; i < _PROCESS_NAME_SIZE__; i++)
	proc_info.processName[PID][i] = 0x20;
	
	return 1;
}


//
// Get a task index by its process name
uint8_t get_task_index(const char process_name[], uint8_t name_length) {
	
	if (name_length > _PROCESS_NAME_SIZE__-1) return 0;
	
	for (uint8_t index=0; index< _PROCESS_LIST_SIZE__; index++) {
		
		if (proc_info.processName[index][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			char nameChar = proc_info.processName[index][a];
			if (nameChar == process_name[a]) count++; else break;
		}
		
		return (index + 1);
	}
	
	return 0;
}



//
// Interrupt timer 0

ISR (TIMER0_OVF_vect) {
	
	for (uint8_t PID=0; PID < _PROCESS_LIST_SIZE__; PID++) {
		
		if (proc_info.processPriority[PID] == 0) continue;
		
		if (proc_info.processCounters[PID] >= proc_info.processPriority[PID]) {
			proc_info.processCounters[PID]=0;
			proc_info.process_pointer_table[PID]();
		} else {
			proc_info.processCounters[PID]++;
			continue;
		}
		
		
		// Task type
		switch (proc_info.processType[PID]) {
			
			// Terminate volatile tasks
			case _TASK_VOLATILE__: {
				for (uint8_t i=0; i < _PROCESS_NAME_SIZE__; i++) proc_info.processName[PID][i] = 0x20;
				proc_info.processType[PID] = 0;
				proc_info.processPriority[PID] = 0;
				proc_info.processCounters[PID] = 0;
				proc_info.process_pointer_table[PID] = 0;
				break;
			}
			
			
			default: break;
			
		}
		
		
	}
	
	return;
}

#endif


void __scheduler_init_(void) {
#ifdef __CORE_SCHEDULER_
	for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
		proc_info.processType[i]     = 0x00;
		proc_info.processPriority[i] = 0x00;
		proc_info.processCounters[i] = 0x00;
		proc_info.process_pointer_table[i] = 0;
		
		for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++)
			proc_info.processName[i][a] = 0x20;
	}
#endif
}

void __scheduler_start(uint8_t timer_priority) {
#ifdef __CORE_SCHEDULER_
	_delay_ms(100);
	
	TCCR0A  = 0x00;          // Normal counter and waveform
	TCCR0B |= (1 << CS10);   // Timer mode with NO prescaler
	TIMSK0 |= (1 << TOIE0);  // Enable timer overflow interrupt
	
	TCNT0 = timer_priority;  // Set timer priority
	sei();                   // Enable interrupts
#endif
}

void __scheduler_stop(void) {
#ifdef __CORE_SCHEDULER_
	_delay_ms(100);
	
	TCCR0A  = 0x00;
	TCCR0B &= ~(1 << CS10);
	TIMSK0 &= ~(1 << TOIE0);
	
	TCNT0 = 0;
#endif
}



#endif

