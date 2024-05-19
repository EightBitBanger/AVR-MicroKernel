#include <avr/io.h>
#include <avr/interrupt.h>

#include <kernel/interrupt.h>

void dummyFunction(void) {};

void (*_timer_comp_a_ptr__)();
void (*_timer_comp_b_ptr__)();


void ClearInterruptFlag(void) {
    
    cli();
    
    return;
}

void SetInterruptFlag(void) {
    
    sei();
    
    return;
}


void TimerCounterStart(void) {
    
    _timer_comp_a_ptr__ = dummyFunction;
    _timer_comp_b_ptr__ = dummyFunction;
	
	// Scheduler timer/counter
	TCCR0A  = _SCHEDULER_TCCRxA;
	TCCR0B  = _SCHEDULER_TCCRxB;
	TIMSK0  = _SCHEDULER_TIMSK;
	OCR0A   = _SCHEDULER_OCR;
	
	// Millisecond clock counter
	TCCR1A  = _CLOCK_TCCRxA;
	TCCR1B  = _CLOCK_TCCRxB;
	TIMSK1  = _CLOCK_TIMSK;
	OCR1A   = _CLOCK_OCR;
	
	return;
}

void TimerCounterStop(void) {
    
	TCCR0B  = 0;
	TCCR1B  = 0;
	TCCR2B  = 0;
	
    return;
}



//
// Interrupt services
//

ISR (TIMER0_COMPA_vect) {
    
    _timer_comp_a_ptr__();
    
    return;
}

ISR (TIMER1_COMPA_vect) {
    
    _timer_comp_b_ptr__();
    
    return;
}

