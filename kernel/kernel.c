#include <kernel/kernel.h>

void _ISR_hardware_service_routine(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 2;
    
    uint8_t interruptVector = 0;
    
    bus_read_io(&bus, 0x00000, &interruptVector);
    
    if (interruptVector == 1)   printc("1", 4);
    if (interruptVector == 2)   printc("2", 4);
    if (interruptVector == 4)   printc("3", 4);
    if (interruptVector == 8)   printc("4", 4);
    if (interruptVector == 16)  printc("5", 4);
    if (interruptVector == 32)  printc("6", 4);
    if (interruptVector == 64)  printc("7", 4);
    if (interruptVector == 128) printc("8", 4);
    
    uint8_t msgInt[] = "INT";
    
    printLn();
    
    print(msgInt, sizeof(msgInt));
    
    printLn();
    
    while(1) {
        
    }
    
    return;
}

