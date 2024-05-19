#ifndef __KERNEL_INTERRUPT_
#define __KERNEL_INTERRUPT_

// Millisecond clock timer
#define _CLOCK_TCCRxA        2
#define _CLOCK_TCCRxB        4
#define _CLOCK_TIMSK         2
#define _CLOCK_OCR           97

// Scheduler counter
#define _SCHEDULER_TCCRxA    2
#define _SCHEDULER_TCCRxB    1
#define _SCHEDULER_TIMSK     2
#define _SCHEDULER_OCR       1


// Timers / counters

void TimerCounterStart(void);
void TimerCounterStop(void);


// Interrupts

void SetInterruptFlag(void);
void ClearInterruptFlag(void);


// Interrupt service routines

uint8_t SetInterruptServiceA(void (*service_ptr)());
uint8_t SetInterruptServiceB(void (*service_ptr)());
uint8_t SetInterruptServiceC(void (*service_ptr)());

#endif
