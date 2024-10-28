#include <avr/io.h>
#include <avr/interrupt.h>

#include <kernel/interrupt.h>
#include <kernel/kernel.h>

void dummyFunction(void) {};

void (*_timer_comp_a_ptr__)() = dummyFunction;
void (*_timer_comp_b_ptr__)() = dummyFunction;
void (*_timer_comp_c_ptr__)() = dummyFunction;

void (*_external_int_ptr__)() = dummyFunction;


void ClearInterruptFlag(void) {
    
    cli();
    
    return;
}

void SetInterruptFlag(void) {
    
    sei();
    
    return;
}


void InterruptStartScheduler(void) {
    
	TCCR0A  = _SCHEDULER_TCCRxA;
	TCCR0B  = _SCHEDULER_TCCRxB;
	TIMSK0  = _SCHEDULER_TIMSK;
	OCR0A   = _SCHEDULER_OCR;
	
	return;
}

void InterruptStartTimeCounter(void) {
    
	TCCR1A  = _CLOCK_TCCRxA;
	TCCR1B  = _CLOCK_TCCRxB;
	TIMSK1  = _CLOCK_TIMSK;
	OCR1A   = _CLOCK_OCR;
	
	return;
}

void InterruptStartHardware(void) {
    
    SetHardwareInterruptService( _ISR_hardware_service_routine );
    
    EICRA = 0b00100000; // Enable INT2 falling edge
    EIMSK = 0b00000100; // Enable INT2 mask
    
    return;
}

void InterruptStopScheduler(void) {
    
    TCCR0B  = 0;
	
	return;
}

void InterruptStopTimerCounter(void) {
    
	TCCR1B  = 0;
	TCCR2B  = 0;
	
    return;
}

void InterruptStopHardware(void) {
    
    EICRA = 0b00000000;
    EIMSK = 0b00000000;
    
    return;
}



//
// Interrupts
//

uint8_t SetInterruptService(uint8_t index, void (*service_ptr)()) {
    
    switch (index) {
    
    case 0:
        _timer_comp_a_ptr__ = service_ptr;
        
    case 1:
        _timer_comp_b_ptr__ = service_ptr;
        
    case 2:
        _timer_comp_c_ptr__ = service_ptr;
        
    }
    
    return 1;
}

uint8_t SetHardwareInterruptService(void (*service_ptr)()) {
    
    _external_int_ptr__ = service_ptr;
    
    return 1;
}

//
// Interrupt service routines
//

ISR (TIMER0_COMPA_vect) {
    
    _timer_comp_a_ptr__();
    
    return;
}

ISR (TIMER1_COMPA_vect) {
    
    _timer_comp_b_ptr__();
    
    return;
}

ISR (TIMER2_COMPA_vect) {
    
    _timer_comp_c_ptr__();
    
    return;
}


ISR (INT2_vect) {
    
    _external_int_ptr__();
    
    return;
}

