#ifndef __TASK_SCHEDULER_
#define __TASK_SCHEDULER_

#define TASK_LIST_SIZE                 10
#define TASK_NAME_LENGTH_MAX           10

#define TASK_TYPE_USER                 'u' // User task
#define TASK_TYPE_SERVICE              's' // System service task
#define TASK_TYPE_VOLATILE             'v' // Volatile task

#define TASK_PRIORITY_BACKGROUND       64
#define TASK_PRIORITY_LOW              32
#define TASK_PRIORITY_NORMAL           16
#define TASK_PRIORITY_HIGH             8
#define TASK_PRIORITY_REALTIME         1

struct ProcessDescriptorTable {
	
	uint8_t  name     [TASK_LIST_SIZE][TASK_NAME_LENGTH_MAX];
	uint8_t  type     [TASK_LIST_SIZE];
	uint16_t priority [TASK_LIST_SIZE];
	uint16_t counter  [TASK_LIST_SIZE];
	void   (*table    [TASK_LIST_SIZE])();
	
};

void schedulerInitiate(void);
void schedulerStart(void);
void schedulerStop(void);

void SchedulerUpdate(void);

uint8_t task_create(uint8_t* name, uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t type);

uint8_t task_destroy(uint8_t index);

uint8_t get_task_index(uint8_t* name, uint8_t name_length);


#endif
