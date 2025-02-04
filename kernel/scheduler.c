#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include <kernel/scheduler.h>
#include <kernel/interrupt.h>
#include <kernel/cstring.h>
#include <kernel/virtual/virtual.h>
#include <kernel/syscalls/print/print.h>

struct Node* ProcessNodeTable = NULL;


volatile uint16_t timer_ms = 0;

uint8_t schedulerIsActive = 0;

extern uint32_t dirProcAddress; // Process directory address 
extern uint32_t procSuperBlock; // Super block address

extern uint32_t __virtual_address_begin__; // Global range
extern uint32_t __virtual_address_end__;

extern uint32_t __heap_begin__; // Process specific heap range
extern uint32_t __heap_end__;


int32_t TaskCreate(uint8_t* name, uint8_t name_length, void(*task_ptr)(uint8_t), uint8_t priority, uint8_t privilege, uint8_t type) {
    
    // Find an available memory range
    uint32_t nextMemoryRange = FindNextAvailableMemoryRange();
    
    // No available memory range found
    if (nextMemoryRange == 0) 
        return -1;
    
    if (name_length > TASK_NAME_LENGTH_MAX)
        name_length = TASK_NAME_LENGTH_MAX;
    
    struct ProcessDescription* newProcPtr = (struct ProcessDescription*)malloc(sizeof(struct ProcessDescription));
    ListAddNode(&ProcessNodeTable, newProcPtr);
    
    // Set process name
    for (uint8_t i = 0; i < TASK_NAME_LENGTH_MAX; i++)
        newProcPtr->name[i] = ' ';
    for (uint8_t i = 0; i < name_length; i++)
        newProcPtr->name[i] = name[i];
    
    // Process parameters
    newProcPtr->type = type;
    newProcPtr->privilege = privilege;
    newProcPtr->priority = priority;
    newProcPtr->counter = priority+1;   // Trigger the initial call immediately
    newProcPtr->block = 0;
    newProcPtr->flags = 0;
    newProcPtr->time_slice = 0;
    
    newProcPtr->heap_begin = nextMemoryRange;
    newProcPtr->heap_end = nextMemoryRange + MEMORY_BLOCK_SIZE;
    
    newProcPtr->function = task_ptr;
    
    // Set user access mode
    uint8_t currentMode = VirtualAccessGetMode();
    switch (privilege) {
        case TASK_PRIVILEGE_KERNEL: { VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_KERNEL); break; }
        case TASK_PRIVILEGE_SERVICE: { VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_SERVICE); break; }
        default:
        case TASK_PRIVILEGE_USER: { VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_USER); break; }
    }
    VirtualBegin();
    
    // Allocate super block
    uint32_t blockAddress = fsDirectoryCreate(name, name_length);
    newProcPtr->block = blockAddress;
    if (newProcPtr->block == 0) {
        ListRemoveNode(&ProcessNodeTable, newProcPtr);
        free(newProcPtr);
        return -1;
    }
    
    struct FSAttribute attrProcFile = { 's', 'r', ' ', ' ' };
    fsFileSetAttributes(blockAddress, &attrProcFile);
    fsDirectoryAddFile(dirProcAddress, blockAddress);
    
    // Restore previous access level
    VirtualEnd();
    VirtualAccessSetMode(currentMode);
    
    return 1;
}


uint8_t TaskDestroy(int32_t index) {
	
	uint32_t listSz = ListGetSize(ProcessNodeTable);
	
	if (index >= listSz) 
        return 0;
    
	struct Node* nodePtr = ListGetNode(ProcessNodeTable, index);
	struct ProcessDescription* proc_desc = nodePtr->data;
	
	ListRemoveNode(&ProcessNodeTable, nodePtr->data);
	
	// Set user access mode
	uint8_t currentMode = VirtualAccessGetMode();
    switch (proc_desc->privilege) {
		case TASK_PRIVILEGE_KERNEL: {VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_KERNEL ); break;}
		case TASK_PRIVILEGE_SERVICE: {VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_SERVICE ); break;}
		default:
        case TASK_PRIVILEGE_USER: {VirtualAccessSetMode( VIRTUAL_ACCESS_MODE_USER ); break;}
	}
	VirtualBegin();
    
	// Free the super block
	fsDirectoryRemoveFile(dirProcAddress, proc_desc->block);
	fsFree(proc_desc->block);
	
	// Restore previous access level
	VirtualEnd();
    VirtualAccessSetMode( currentMode );
	
	// Free the process descriptor
	free(proc_desc);
	
	return 1;
}

int32_t TaskFind(uint8_t* name, uint8_t name_length) {
	
	if (name_length > TASK_NAME_LENGTH_MAX)
        name_length = TASK_NAME_LENGTH_MAX;
	
	uint32_t index = -1;
	
	uint32_t numberOfTasks = ListGetSize(ProcessNodeTable);
	for (uint32_t i=0; i < numberOfTasks; i++) {
        
        struct Node* nodePtr = ListGetNode(ProcessNodeTable, i);
        struct ProcessDescription* proc = nodePtr->data;
        
        // Check task name
        if (StringCompare(name, name_length, proc->name, FILE_NAME_LENGTH) == 0) 
            continue;
        
        index = i;
        break;
	}
	
    return index;
}


uint8_t GetProcInfo(int32_t index, struct ProcessDescription* proc_desc) {
	
	struct Node* nodePtr = ListGetNode(ProcessNodeTable, index);
	proc_desc = nodePtr->data;
	
	return 1;
}

uint32_t FindNextAvailableMemoryRange(void) {
    uint32_t currentAddress = MEMORY_START;
    
    struct Node* temp = ProcessNodeTable;
    while (temp != NULL) {
        struct ProcessDescription* proc = (struct ProcessDescription*)temp->data;
        
        // Found an available range
        if (currentAddress < proc->heap_begin) 
            return currentAddress;
        
        currentAddress = proc->heap_end;
        temp = temp->next;
    }
    
    // Check if there's space at the end of the memory
    if (currentAddress + MEMORY_BLOCK_SIZE <= MEMORY_END) {
        return currentAddress;
    }
    
    // No available memory range found
    return 0;
}


//
// Interrupt service routines
//

void SchedulerStart(void) {
	
	if (schedulerIsActive == 1) 
        return;
    
    schedulerIsActive = 1;
	
	// Set the ISRs
	SetInterruptService(0, _ISR_SCHEDULER_TIMER__);
    SetInterruptService(1, _ISR_SCHEDULER_MAIN__);
    
	return;
}


void SchedulerStop(void) {
	
	if (schedulerIsActive == 0) 
        return;
    
	schedulerIsActive = 0;
	
	return;
}



//
// Scheduler entry point

volatile uint32_t procIndex=0; 

void _ISR_SCHEDULER_MAIN__(void) {
    
    // Check no tasks running
    uint32_t numberOfTasks = ListGetSize(ProcessNodeTable);
    if (numberOfTasks == 0) 
        return;
    
    struct Node* nodePtr = ListGetNode(ProcessNodeTable, procIndex);
    struct ProcessDescription* proc_info = nodePtr->data;
    
    int8_t currentMode = VirtualAccessGetMode();
    
    // Set privilege level
    switch (proc_info->privilege) {
        case TASK_PRIVILEGE_KERNEL: { VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_KERNEL); break; }
        case TASK_PRIVILEGE_SERVICE: { VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_SERVICE); break; }
        default:
        case TASK_PRIVILEGE_USER: { VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_USER); break; }
    }
    
    // Set the reference to the process super block
    // for memory allocation using new and delete
    procSuperBlock = proc_info->block;
    
    // Set process heap range
    __heap_begin__ = proc_info->heap_begin;
    __heap_end__   = proc_info->heap_end;
    
    // Process task priority
    if (proc_info->counter > proc_info->priority) {
        proc_info->counter = 0;
        
        uint8_t event = EVENT_NOMESSAGE;
        
        // Check initial run
        if (proc_info->flags == 0) {
            proc_info->flags = 1;
            
            event = EVENT_INITIATE;
        }
        
        // Reset the time slice
        proc_info->time_slice = 0;
        
        // Call the TSR program
        SetInterruptFlag();
        proc_info->function(event);
        
    } else {
        
        // Increment the priority counter
        proc_info->counter++;
    }
    
    // Process task type
    switch (proc_info->type) {
        
        case TASK_TYPE_VOLATILE: {
            
            TaskDestroy(procIndex);
            procIndex = 0;
        }
    }
    
    // Reset the heap access range
    __heap_begin__ = 0x00000000;
    __heap_end__   = 0xffffffff;
    
    // Free the super block
    procSuperBlock = 0;
    
    VirtualAccessSetMode(currentMode);
    
    procIndex++;
    
    // Check process counter index overflow
    if (procIndex >= numberOfTasks) 
        procIndex = 0;
    
    return;
}


// System base timer and task watchdog

volatile uint32_t procTimeSlice=0; 

void _ISR_SCHEDULER_TIMER__(void) {
    
    timer_ms++;
    
    // Check no tasks running
    uint32_t numberOfTasks = ListGetSize(ProcessNodeTable);
    if (numberOfTasks == 0) 
        return;
    
    struct Node* nodePtr = ListGetNode(ProcessNodeTable, procIndex);
    struct ProcessDescription* proc_info = nodePtr->data;
    
    // Increment the time slice
    proc_info->time_slice++;
    
    
    // Check timeout
    if (proc_info->time_slice > 100) {
        
        TaskDestroy(procIndex);
        
        procIndex = 0;
        return;
    }
    
    return;
}

