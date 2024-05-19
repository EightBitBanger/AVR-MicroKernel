#include <kernel/kernel.h>

void _ISR_hardware_service_routine(void) {
    
    printc("fuck", 5);
    printLn();
    
    while(1) {
        continue;
    }
    
    return;
}

