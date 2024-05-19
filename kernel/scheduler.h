#ifndef __TASK_SCHEDULER_
#define __TASK_SCHEDULER_

#define TASK_LIST_SIZE                 10
#define TASK_NAME_LENGTH_MAX           10

#define TASK_TYPE_USER                 'u' // User task
#define TASK_TYPE_SERVICE              's' // System service task
#define TASK_TYPE_VOLATILE             'v' // Volatile task

#define TASK_PRIORITY_HALT             0
#define TASK_PRIORITY_BACKGROUND       8192
#define TASK_PRIORITY_LOW              1024
#define TASK_PRIORITY_NORMAL           256
#define TASK_PRIORITY_HIGH             64
#define TASK_PRIORITY_REALTIME         32

struct ProcessDescriptorTable {
	
	uint8_t  name     [TASK_LIST_SIZE][TASK_NAME_LENGTH_MAX];
	uint8_t  type     [TASK_LIST_SIZE];
	uint16_t priority [TASK_LIST_SIZE];
	uint16_t counter  [TASK_LIST_SIZE];
	void   (*table    [TASK_LIST_SIZE])();
	
};

uint8_t task_create(uint8_t* name, uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t type);

uint8_t task_destroy(uint8_t index);

uint8_t task_find(uint8_t* name, uint8_t name_length);

uint8_t task_kill(uint8_t* name, uint8_t name_length);


void schedulerInitiate(void);
void schedulerStart(void);
void schedulerStop(void);

void SchedulerUpdate(void);

#endif
