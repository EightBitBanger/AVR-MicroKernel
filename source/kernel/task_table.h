#ifndef __TASK_SCHEDULER_TABLE_
#define __TASK_SCHEDULER_TABLE_


struct ProcessDescriptorTable {
	
	uint8_t  name     [TASK_LIST_SIZE][TASK_NAME_LENGTH_MAX];
	uint8_t  type     [TASK_LIST_SIZE];
	uint16_t priority [TASK_LIST_SIZE];
	uint16_t counter  [TASK_LIST_SIZE];
	void   (*table    [TASK_LIST_SIZE])();
	
}volatile static proc_info;


uint8_t task_create(const char* name, uint8_t name_length, void(*task_ptr)(), uint32_t priority, uint8_t type) {
	
	if (name_length > TASK_NAME_LENGTH_MAX)
	name_length = TASK_NAME_LENGTH_MAX;
	
	uint8_t index;
	for (index=0; index < TASK_LIST_SIZE; index++)
	if (proc_info.priority[index] == 0) break;
	
	if (index >= TASK_LIST_SIZE) return 0;
	
	for (uint8_t a=0; a < name_length-1; a++)
	proc_info.name[index][a] = name[a];
	
	proc_info.type[index]      = type;
	proc_info.priority[index]  = priority;
	proc_info.counter[index]   = 0;
	proc_info.table[index]     = task_ptr;
	
	return (index + 1);
}


uint8_t task_destroy(uint8_t index) {
	
	uint8_t PID = index - 1;
	
	if (PID > TASK_LIST_SIZE) return 0;
	if (proc_info.name[PID][0] == 0x20) return 0;
	
	proc_info.type[PID]      = 0;
	proc_info.priority[PID]  = 0;
	proc_info.counter[PID]   = 0;
	proc_info.table[PID]     = 0;
	
	for (uint8_t i=0; i < TASK_NAME_LENGTH_MAX; i++)
	proc_info.name[PID][i] = 0x20;
	
	return 1;
}


uint8_t get_task_index(const char* name, uint8_t name_length) {
	
	if (name_length > TASK_NAME_LENGTH_MAX)
	name_length = TASK_NAME_LENGTH_MAX;
	
	char task_name[name_length];
	for (uint8_t i=0; i < name_length-1; i++)
	task_name[i] = name[i];
	
	for (uint8_t index=0; index< TASK_LIST_SIZE; index++) {
		
		if (proc_info.name[index][0] == 0x20) continue;
		
		char list_task_name[name_length];
		for (uint8_t i=0; i < name_length-1; i++)
		list_task_name[i] = proc_info.name[index][i];
		
		if (strcmp(task_name, list_task_name, name_length) == 1)
		return (index + 1);
		
	}
	
	return 0;
}

ISR (TIMER0_COMPA_vect) {
	
	timer_ms++;
	
}

ISR (TIMER1_COMPA_vect) {
	
	for (uint8_t PID=0; PID < TASK_LIST_SIZE; PID++) {
		
		if (proc_info.priority[PID] == 0) continue;
		
		if (proc_info.counter[PID] >= proc_info.priority[PID]) {
			proc_info.counter[PID]=0;
			proc_info.table[PID]();
		} else {
			proc_info.counter[PID]++;
			continue;
		}
		
		switch (proc_info.type[PID]) {
			
			case TASK_TYPE_VOLATILE: {
				
				for (uint8_t i=0; i < TASK_NAME_LENGTH_MAX; i++)
				proc_info.name[PID][i] = 0x20;
				
				proc_info.type[PID]      = 0;
				proc_info.priority[PID]  = 0;
				proc_info.counter[PID]   = 0;
				proc_info.table[PID]     = 0;
				
				break;
			}
			
			case TASK_TYPE_SERVICE: {
				
				break;
			}
			
			case TASK_TYPE_USER: {
				
				break;
			}
			
			default: break;
			
		}
		
		
	}
	return;
}

void __scheduler_init_(void) {
	
#ifdef __CORE_SCHEDULER_
	
	for (uint8_t i=0; i < TASK_LIST_SIZE; i++) {
		proc_info.type[i]     = 0x00;
		proc_info.priority[i] = 0x00;
		proc_info.counter[i]  = 0x00;
		proc_info.table[i]    = 0;
		
		for (uint8_t a=0; a < TASK_NAME_LENGTH_MAX; a++)
		proc_info.name[i][a] = 0x20;
	}
	
#endif
	
	return;
}


void __scheduler_start(void) {
	
#ifdef __CORE_SCHEDULER_
	
	cli();
	
	// Start the millisecond clock
	TCCR1A  = _CLOCK_TCCRxA;
	TCCR1B  = _CLOCK_TCCRxB;
	TIMSK1  = _CLOCK_TIMSK;
	OCR1A   = _CLOCK_OCR;
	
	// Start the scheduler
	TCCR0A  = _SCHEDULER_TCCRxA;
	TCCR0B  = _SCHEDULER_TCCRxB;
	TIMSK0  = _SCHEDULER_TIMSK;
	OCR0A   = _SCHEDULER_OCR;
	
	sei();
	
#endif
	
	return;
}


void __scheduler_stop(void) {
	
#ifdef __CORE_SCHEDULER_
	
	TCCR0B  = 0;
	
#endif
	
	return;
}

#endif

