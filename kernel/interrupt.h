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

void InterruptStartTimeCounter(void);
void InterruptStartScheduler(void);
void InterruptStartHardware(void);

void InterruptStopTimerCounter(void);
void InterruptStopScheduler(void);
void InterruptStopHardware(void);


// Interrupts

void SetInterruptFlag(void);
void ClearInterruptFlag(void);


// Interrupt service routines

uint8_t SetInterruptService(uint8_t index, void (*service_ptr)());

// Hardware interrupt service routines

uint8_t SetHardwareInterruptService(void (*service_ptr)());

#endif
