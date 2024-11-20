#include <avr/io.h>
#include <avr/interrupt.h>

#include <kernel/scheduler.h>
#include <kernel/interrupt.h>
#include <kernel/cstring.h>
#include <kernel/virtual/virtual.h>
#include <kernel/syscalls/print/print.h>

volatile struct ProcessNodeTable proc_info;

volatile uint64_t timer_ms = 0;

uint8_t schedulerIsActive = 0;


uint8_t TaskCreate(uint8_t* name, uint8_t name_length, void(*task_ptr)(uint8_t), uint8_t priority, uint8_t privilege, uint8_t type) {
	
	if (name_length > TASK_NAME_LENGTH_MAX)
		name_length = TASK_NAME_LENGTH_MAX;
	
	// Find an empty slot
	uint8_t index;
	for (index=0; index < TASK_LIST_SIZE; index++) {
		
		if (proc_info.type[index] == TASK_TYPE_UNKNOWN) 
            break;
	}
	
	// MAX task count
	if (index == TASK_LIST_SIZE-1) 
        return 0;
    
	proc_info.type[index]      = type;
	proc_info.privilege[index] = privilege;
	proc_info.priority[index]  = priority;
	proc_info.counter[index]   = 0;
	proc_info.block[index]     = 0;
	proc_info.function[index]  = task_ptr;
	
	// Prepare the memory block
	uint8_t currentMode = VirtualAccessGetMode();
    
    if (privilege == TASK_PRIVILEGE_KERNEL)  VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_KERNEL );
    if (privilege == TASK_PRIVILEGE_SERVICE) VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_SERVICE );
    if (privilege == TASK_PRIVILEGE_USER)    VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_USER );
    
    // Allocate a super block
	proc_info.block[index] = fsAllocate(64);
	
	VirtualBegin();
	
	fsFileSetName(proc_info.block[index], name, name_length);
	
	struct FSAttribute attrib = {'s', 'r', 'w', 'd'};
	
	fsFileSetAttributes(proc_info.block[index], &attrib);
	
	fsDirectorySetNumberOfFiles(proc_info.block[index], 0);
	
	VirtualEnd();
	
	VirtualAccessSetMode(currentMode);
	
	return (index + 1);
}


uint8_t TaskDestroy(uint8_t index) {
	
	uint8_t PID = index - 1;
	
	if (PID > TASK_LIST_SIZE) 
        return 0;
    
	if (proc_info.type[PID] == TASK_TYPE_UNKNOWN) 
        return 0;
	
	proc_info.type[PID] = TASK_TYPE_VOLATILE;
    
	return 1;
}

uint32_t TaskFind(uint8_t* name, uint8_t name_length) {
	
	if (name_length > TASK_NAME_LENGTH_MAX)
        name_length = TASK_NAME_LENGTH_MAX;
	
	for (uint8_t index=0; index < TASK_LIST_SIZE; index++) {
		
		if (proc_info.type[index] == 0x00) 
            continue;
		
		uint32_t blockAddress = proc_info.block[index];
		
		VirtualBegin();
		
		uint8_t list_task_name[FILE_NAME_LENGTH];
		fsFileGetName(blockAddress, list_task_name);
		
		if (StringCompare(name, name_length, list_task_name, FILE_NAME_LENGTH) == 1) {
            VirtualEnd();
            return (index + 1);
        }
        
        VirtualEnd();
        
	}
	
	return 0;
}


uint8_t TaskKill(uint8_t* name, uint8_t name_length) {
	
	uint32_t index = TaskFind(name, name_length);
	
	if (index == 0) 
        return 0;
    
	proc_info.type[index - 1] = TASK_TYPE_VOLATILE;
	
	// Kill halted tasks
	if (proc_info.priority[index - 1] == TASK_PRIORITY_HALT) {
        
        uint8_t ind = index - 1;
        
        fsFree( proc_info.block[ind] );
        
        proc_info.type[ind]      = TASK_TYPE_UNKNOWN;
        proc_info.privilege[ind] = 0;
        proc_info.priority[ind]  = 0;
        proc_info.counter[ind]   = 0;
        proc_info.flags[ind]     = 0;
        proc_info.function[ind]  = 0;
        proc_info.block[ind]     = 0;
        
    }
    
	return 1;
}

uint8_t GetProcInfo(uint8_t index, struct ProcessDescription* info) {
	
	if (proc_info.type[index] == TASK_TYPE_UNKNOWN) 
        return 0;
    
	info->type      = proc_info.type[index];
	info->privilege = proc_info.privilege[index];
	info->priority  = proc_info.priority[index];
	info->counter   = proc_info.counter[index];
	info->flags     = proc_info.flags[index];
	info->function  = proc_info.function[index];
	info->block     = proc_info.block[index];
	
	return 1;
}



//
// Interrupt service routines
//

volatile uint8_t PID=0; 

void SchedulerInit(void) {
	
	for (uint8_t i=0; i < TASK_LIST_SIZE; i++) {
		
		proc_info.type[i]      = TASK_TYPE_UNKNOWN;
		proc_info.privilege[i] = 0;
		proc_info.priority[i]  = 0;
		proc_info.counter[i]   = 0;
		proc_info.flags[i]     = 0;
		proc_info.function[i]  = (void(*)(uint8_t))nullfunc;
		
	}
	
	return;
}


void SchedulerStart(void) {
	
	if (schedulerIsActive == 1) 
        return;
    
    schedulerIsActive = 1;
	
	ClearInterruptFlag();
	
	// Set the ISRs
	SetInterruptService(0, _ISR_SCHEDULER_MAIN__ );
    SetInterruptService(1, _ISR_SCHEDULER_TIMER__);
    
    SetInterruptFlag();
	
	return;
}


void SchedulerStop(void) {
	
	if (schedulerIsActive == 0) 
        return;
    
	schedulerIsActive = 0;
	
	return;
}


//
// Interrupt service routines
//

void _ISR_SCHEDULER_MAIN__(void) {
    
    if (PID >= TASK_LIST_SIZE) {
        PID = 0;
    }
    
	if (proc_info.type[PID] == TASK_TYPE_UNKNOWN) {
        
        PID++;
        
        return;
	}
	
	int8_t currentMode = VirtualAccessGetMode();
	
	//
	// Set privilege level
    
	switch (proc_info.privilege[PID]) {
		
		case TASK_PRIVILEGE_KERNEL:  {VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_KERNEL ); break;}
		
		case TASK_PRIVILEGE_SERVICE: {VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_SERVICE ); break;}
		
		default:
        case TASK_PRIVILEGE_USER:    {VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_USER ); break;}
        
	}
	
	
    // Check type
    
	switch (proc_info.type[PID]) {
		
		case TASK_TYPE_VOLATILE: {
			
            proc_info.function[PID]( EVENT_SHUTDOWN );
            
			proc_info.type[PID]      = TASK_TYPE_UNKNOWN;
			proc_info.privilege[PID] = 0;
			proc_info.priority[PID]  = 0;
			proc_info.counter[PID]   = 0;
			proc_info.function[PID]  = (void(*)(uint8_t))nullfunc;
            
            // Free the super block
            VirtualBegin();
            
            fsFree( proc_info.block[PID] );
            
            VirtualEnd();
            
            proc_info.block[PID] = 0;
            
		}
		
		case TASK_TYPE_VOLATILE_PROMPT: {
			
            proc_info.function[PID]( EVENT_SHUTDOWN );
            
			proc_info.type[PID]      = TASK_TYPE_UNKNOWN;
			proc_info.privilege[PID] = 0;
			proc_info.priority[PID]  = 0;
			proc_info.counter[PID]   = 0;
			proc_info.function[PID]  = (void(*)(uint8_t))nullfunc;
            
            // Free the super block
            VirtualBegin();
            
            fsFree( proc_info.block[PID] );
            
            VirtualEnd();
            
            proc_info.block[PID] = 0;
            
            // Drop a command prompt
            printPrompt();
			
		}
		
	}
	
	// Call the task function
	
	if (proc_info.counter[PID] > proc_info.priority[PID]) {
		
		proc_info.counter[PID]=0;
		
		if (proc_info.flags[PID] == 0) {
            
            proc_info.function[PID]( EVENT_INITIATE );
            
            proc_info.flags[PID] = 1;
            
		} else {
            
            proc_info.function[PID]( EVENT_NOMESSAGE );
            
        }
        
		VirtualAccessSetMode( currentMode );
        
	} else {
		
        VirtualAccessSetMode( currentMode );
        
		proc_info.counter[PID]++;
		
		PID++;
        
		return;
	}
	
	
	PID++;
    
    return;
}


void _ISR_SCHEDULER_TIMER__(void) {
    
    timer_ms++;
    
    return;
}

