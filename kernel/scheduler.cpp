#include <avr/io.h>
#include <avr/interrupt.h>

#include "scheduler.h"

struct ProcessScheduler {
	
	uint8_t  processName[_PROCESS_LIST_SIZE__][_PROCESS_NAME_SIZE__];
	uint8_t  processType[_PROCESS_LIST_SIZE__];
	uint16_t processPriority[_PROCESS_LIST_SIZE__];
	uint16_t processCounters[_PROCESS_LIST_SIZE__];
	void (*process_pointer_table[_PROCESS_LIST_SIZE__])();
	
};
ProcessScheduler scheduler;

void scheduler_initiate(void) {
	for (uint8_t i=0; i < _PROCESS_LIST_SIZE__; i++) {
		
		for (uint8_t a=0; a < _PROCESS_NAME_SIZE__; a++)
			scheduler.processName[i][a] = 0x20;
		
		scheduler.processType[i]     = 0x00;
		scheduler.processPriority[i] = 0x00;
		scheduler.processCounters[i] = 0x00;
		scheduler.process_pointer_table[i] = 0;
	}
}

void scheduler_start(uint8_t timer_priority) {
	
	TCCR0A  = 0x00;          // Normal counter and waveform
	TCCR0B |= (1 << CS10);   // Timer mode with NO prescaler
	TIMSK0 |= (1 << TOIE0);  // Enable timer overflow interrupt
	
	TCNT0 = timer_priority;  // Set timer priority rate
	sei();
}

void scheduler_stop(void) {
	
	TCCR0A  = 0x00;
	TCCR0B &= ~(1 << CS10);
	TIMSK0 &= ~(1 << TOIE0);
	
	TCNT0 = 0;
}

uint8_t createProcess(const char name[], uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t task_type) {
	
	if ((priority == 0) || (name_length > _PROCESS_NAME_SIZE__)) return 0;
	
	// Find an available slot
	uint8_t index;
	for (index=0; index <= _PROCESS_LIST_SIZE__; index++)
		if (scheduler.processPriority[index] == 0) break; else continue;
	
	// No free slots
	if (index == _PROCESS_LIST_SIZE__) return 0;
	
	// Launch the new process
	for (uint8_t a=0; a < name_length; a++)
	scheduler.processName[index][a] = name[a];
	
	scheduler.processType[index]           = task_type;
	scheduler.processPriority[index]       = priority;
	scheduler.processCounters[index]       = 0;
	scheduler.process_pointer_table[index] = task_ptr;
	
	return 1;
}

uint8_t getProcessIndex(const char process_name[], uint8_t name_length) {
	
	if (name_length > _PROCESS_NAME_SIZE__) return 0;
	
	// Function look up
	for (uint8_t index=0; index< _PROCESS_LIST_SIZE__; index++) {
		
		if (scheduler.processName[index][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			char nameChar = scheduler.processName[index][a];
			if (nameChar == process_name[a]) count++; else break;
		}
		
		// Return the index
		return index;
	}
	
	return 0;
}

uint8_t killProcess(const char process_name[], uint8_t name_length) {
	
	// Find the task
	uint8_t PID = getProcessIndex(process_name, name_length);
	
	// Kill the task
	scheduler.processType[PID]     = 0;
	scheduler.processPriority[PID] = 0;
	scheduler.processCounters[PID] = 0;
	scheduler.process_pointer_table[PID] = 0;
	
	for (uint8_t i=0; i < _PROCESS_NAME_SIZE__; i++)
	scheduler.processName[PID][i] = 0x20;
	
	return 1;
}

// Interrupt scheduler
ISR (TIMER0_OVF_vect) {
	
	for (uint8_t PID=0; PID < _PROCESS_LIST_SIZE__; PID++) {
		
		if (scheduler.processPriority[PID] == 0) continue;
		
		// Check process timer counter
		if (scheduler.processCounters[PID] >= scheduler.processPriority[PID]) {
			// Yield control to the process
			scheduler.processCounters[PID]=0;
			scheduler.process_pointer_table[PID]();
		} else {
			scheduler.processCounters[PID]++;
			continue;
		}
		
		switch (scheduler.processType[PID]) {
			
			case _TASK_VOLATILE__: {
				// Remove process
				for (uint8_t i=0; i < _PROCESS_NAME_SIZE__; i++) scheduler.processName[PID][i] = 0x20;
				scheduler.processType[PID] = 0;
				scheduler.processPriority[PID] = 0;
				scheduler.processCounters[PID] = 0;
				scheduler.process_pointer_table[PID] = 0;
				break;
			}
			
			
			default: break;
			
		}
		
		
	}
	
	return;
}


