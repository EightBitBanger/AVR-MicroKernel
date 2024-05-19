#ifndef __TASK_SCHEDULER_
#define __TASK_SCHEDULER_

#include <avr/interrupt.h>

#define TASK_LIST_SIZE        16
#define TASK_NAME_LENGTH_MAX  10

struct ProcessDescriptorTable {
	
	uint8_t  name     [TASK_LIST_SIZE][TASK_NAME_LENGTH_MAX];
	uint8_t  type     [TASK_LIST_SIZE];
	uint16_t priority [TASK_LIST_SIZE];
	uint16_t counter  [TASK_LIST_SIZE];
	void   (*table    [TASK_LIST_SIZE])();
	
};







#endif
