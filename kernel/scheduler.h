#ifndef __TASK_SCHEDULER_
#define __TASK_SCHEDULER_

#define TASK_LIST_SIZE                 40
#define TASK_NAME_LENGTH_MAX           10

#define TASK_TYPE_USER                 'u' // User task
#define TASK_TYPE_SERVICE              's' // System service task
#define TASK_TYPE_VOLATILE             'v' // Volatile task (run once)
#define TASK_TYPE_KERNEL               'k' // Kernel task
#define TASK_TYPE_UNKNOWN              0   // No type defined

#define TASK_PRIORITY_HALT             0
#define TASK_PRIORITY_BACKGROUND       64
#define TASK_PRIORITY_LOW              32
#define TASK_PRIORITY_NORMAL           16
#define TASK_PRIORITY_HIGH             8
#define TASK_PRIORITY_REALTIME         1


struct ProcessDescription {
	
	uint8_t  name[TASK_NAME_LENGTH_MAX];
	uint8_t  type;
	uint16_t priority;
	uint16_t counter;
	void   (*table)();
	
};

struct ProcessDescriptorTable {
	
	uint8_t  name     [TASK_LIST_SIZE][TASK_NAME_LENGTH_MAX];
	uint8_t  type     [TASK_LIST_SIZE];
	uint16_t priority [TASK_LIST_SIZE];
	uint16_t counter  [TASK_LIST_SIZE];
	void   (*table    [TASK_LIST_SIZE])();
	
};

uint8_t TaskCreate(uint8_t* name, uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t type);

uint8_t TaskDestroy(uint8_t index);

uint8_t TaskFind(uint8_t* name, uint8_t name_length);

uint8_t TaskKill(uint8_t* name, uint8_t name_length);

uint8_t GetProcInfo(uint8_t index, struct ProcessDescription* info);


void SchedulerInit(void);
void SchedulerStart(void);
void SchedulerStop(void);


void _ISR_SCHEDULER_MAIN__(void);
void _ISR_SCHEDULER_TIMER__(void);

#endif
