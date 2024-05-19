#include <avr/io.h>

#include <kernel/cstring.h>

#include <kernel/scheduler.h>

volatile struct ProcessDescriptorTable proc_info;
uint8_t schedulerIsActive = 0;

uint8_t task_create(uint8_t* name, uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t type) {
	
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
	if (proc_info.name[PID][0] == ' ') return 0;
	
	proc_info.type[PID]      = 0;
	proc_info.priority[PID]  = 0;
	proc_info.counter[PID]   = 0;
	proc_info.table[PID]     = 0;
	
	for (uint8_t i=0; i < TASK_NAME_LENGTH_MAX; i++)
		proc_info.name[PID][i] = ' ';
	
	return 1;
}

uint8_t task_find(uint8_t* name, uint8_t name_length) {
	
	if (name_length > TASK_NAME_LENGTH_MAX)
        name_length = TASK_NAME_LENGTH_MAX;
	
	uint8_t task_name[name_length];
	for (uint8_t i=0; i < name_length-1; i++)
        task_name[i] = name[i];
	
	for (uint8_t index=0; index < TASK_LIST_SIZE; index++) {
		
		if (proc_info.name[index][0] == ' ') continue;
		
		uint8_t list_task_name[name_length];
		for (uint8_t i=0; i < name_length-1; i++)
            list_task_name[i] = proc_info.name[index][i];
		
		if (string_compare(task_name, list_task_name, TASK_NAME_LENGTH_MAX) == 1) {
            
            return (index + 1);
        }
        
	}
	
	return 0;
}


uint8_t task_kill(uint8_t* name, uint8_t name_length) {
	
	if (name_length > TASK_NAME_LENGTH_MAX)
        name_length = TASK_NAME_LENGTH_MAX;
	
	uint8_t task_name[name_length];
	for (uint8_t i=0; i < name_length-1; i++)
        task_name[i] = name[i];
	
	for (uint8_t index=0; index < TASK_LIST_SIZE; index++) {
		
		if (proc_info.name[index][0] == ' ') continue;
		
		uint8_t list_task_name[name_length];
		for (uint8_t i=0; i < name_length-1; i++)
            list_task_name[i] = proc_info.name[index][i];
		
		if (string_compare(task_name, list_task_name, TASK_NAME_LENGTH_MAX) == 1) {
            
            for (uint8_t i=0; i < TASK_NAME_LENGTH_MAX; i++) 
                proc_info.name[index][i] = ' ';
            
            proc_info.counter[index] = 0;
            proc_info.priority[index] = 0;
            proc_info.type[index] = 0;
            
            return (index + 1);
        }
        
	}
	
	return 0;
}


//
// Interrupt service routines
//

void SchedulerUpdate(void) {
	
	if (schedulerIsActive == 0) 
        return;
    
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

void schedulerInitiate(void) {
	
	for (uint8_t i=0; i < TASK_LIST_SIZE; i++) {
		proc_info.type[i]     = 0x00;
		proc_info.priority[i] = 0x00;
		proc_info.counter[i]  = 0x00;
		proc_info.table[i]    = 0;
		
		for (uint8_t a=0; a < TASK_NAME_LENGTH_MAX; a++)
            proc_info.name[i][a] = ' ';
	}
	
	return;
}


void schedulerStart(void) {
	schedulerIsActive = 1;
	return;
}


void schedulerStop(void) {
	schedulerIsActive = 0;
	return;
}
