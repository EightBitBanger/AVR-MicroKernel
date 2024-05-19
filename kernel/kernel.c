#include <kernel/kernel.h>
#include <kernel/delay.h>

#define INT_VECT_0  254
#define INT_VECT_1  253
#define INT_VECT_2  251
#define INT_VECT_3  247
#define INT_VECT_4  239
#define INT_VECT_5  223
#define INT_VECT_6  191
#define INT_VECT_7  127

void _ISR_hardware_service_routine(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    uint8_t iVect = 0;
    
    bus_read_io(&bus, 0x00000, &iVect);
    
    if (iVect == INT_VECT_0) {printc("1", 2); printLn(); printPrompt(); _delay_ms(10);}
    if (iVect == INT_VECT_1) {printc("2", 2); printLn(); printPrompt(); _delay_ms(10);}
    if (iVect == INT_VECT_2) {printc("3", 2); printLn(); printPrompt(); _delay_ms(10);}
    if (iVect == INT_VECT_3) {printc("4", 2); printLn(); printPrompt(); _delay_ms(10);}
    if (iVect == INT_VECT_4) {printc("5", 2); printLn(); printPrompt(); _delay_ms(10);}
    if (iVect == INT_VECT_5) {printc("6", 2); printLn(); printPrompt(); _delay_ms(10);}
    if (iVect == INT_VECT_6) {printc("7", 2); printLn(); printPrompt(); _delay_ms(10);}
    if (iVect == INT_VECT_7) {printc("8", 2); printLn(); printPrompt(); _delay_ms(10);}
    
    return;
}

