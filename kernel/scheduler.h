#ifndef ____TASK_SCHEDULER__
#define ____TASK_SCHEDULER__

#include <avr/interrupt.h>

// Task list
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

// TCCRxA   0 - Normal counter and waveform
//          2 - No OC0A, no OC0B, with CTC
//          
// TCCRxB   0 - Timer stopped
//          1 - No prescaler
//          2 - div8
//          3 - div64
//          4 - div256
//          5 - div1024
//          
// TIMSK    1 - Timer overflow int
//          2 - Timer output Compare A Match
//          3 - Timer output Compare B Match
//          
// OCR      24,000,000 / 256 / (1 + 93)  = 997Kh
//          25,000,000 / 256 / (1 + 97)  = 996Kh


// Millisecond clock timer
#define _CLOCK_TCCRxA    2
#define _CLOCK_TCCRxB    4
#define _CLOCK_TIMSK     2
#define _CLOCK_OCR       97

// Scheduler counter
#define _SCHEDULER_TCCRxA    2
#define _SCHEDULER_TCCRxB    1
#define _SCHEDULER_TIMSK     2
#define _SCHEDULER_OCR       1


// Schedule a new task
uint8_t task_create(const char* name, uint8_t name_length, void(*task_ptr)(), uint32_t priority, uint8_t type);
// Remove a running task
uint8_t task_destroy(uint8_t PID);
// Get a task index by its name
uint8_t get_task_index(const char* name, uint8_t name_length);


// Initiate the scheduler
void __scheduler_init_();
// Start the scheduler timer/counter
void __scheduler_start(void);
// Stop the scheduler timer/counter
void __scheduler_stop(void);


// Millisecond timer
static volatile uint32_t timer_ms  = 0;


#endif

