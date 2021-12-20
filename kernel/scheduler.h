//
// Cooperative process scheduler

#ifndef __PROCESS_SCHEDULER__
#define __PROCESS_SCHEDULER__

#include <avr/io.h>
#include<avr/interrupt.h>

#define _PROCESS_LIST_SIZE__  16
#define _PROCESS_NAME_SIZE__  10

#define _TASK_USER__      0x75 // Process task is a user program
#define _TASK_VOLATILE__  0x76 // Process task will terminate after execution
#define _TASK_SERVICE__   0x73 // Process task is a system service routine


// Initiate the scheduler system
void scheduler_initiate(void);

// Start the scheduler with the given timer priority
void scheduler_start(uint8_t timer_priority);

// Stop the scheduler
void scheduler_stop(void);

// Schedule a new process
uint8_t createProcess(const char name[], uint8_t name_length, void(*task_ptr)(), uint16_t priority, uint8_t task_type);

// Get a process index by its process name
uint8_t getProcessIndex(const char process_name[], uint8_t name_length);

// Stop a running process
uint8_t killProcess(const char process_name[], uint8_t name_length);

#endif

