#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/cap.h>

void functionCAP(uint8_t* param, uint8_t param_length) {
    
    uint32_t deviceCapacity = fsGetDeviceCapacity();
    
    uint8_t integer[10];
    
    uint8_t place = int_to_string( deviceCapacity, integer);
    
    print( &integer[0], place + 1 );
    
    printLn();
    
    return;
}

void registerCommandCAP(void) {
    
    uint8_t capCommandName[] = "cap";
    
    ConsoleRegisterCommand(capCommandName, sizeof(capCommandName), functionCAP);
    
    return;
}
