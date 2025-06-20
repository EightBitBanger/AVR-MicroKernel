<<<<<<< HEAD
#include <kernel/syscalls/timer/timer.h>

extern volatile uint64_t system_timer_ms;

uint64_t time_ms(void) {
    
    return system_timer_ms;
}
=======
#include <kernel/syscalls/timer/timer.h>

extern volatile uint64_t system_timer_ms;

uint64_t time_ms(void) {
    
    return system_timer_ms;
}
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
