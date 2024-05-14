#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/cap.h>

void functionCAP(uint8_t* param, uint8_t param_length) {
    
    uint32_t deviceCapacity = fsGetDeviceCapacity();
    
    uint8_t integer[10];
    
    uint8_t place = int_to_string( deviceCapacity, integer);
    
    uint8_t capacityMsg[] = " bytes total";
    
    print( &integer[0], place + 1 );
    print( &capacityMsg[0], sizeof(capacityMsg) );
    
    printLn();
    
    return;
}

void registerCommandCAP(void) {
    
    uint8_t capCommandName[] = "cap";
    
    ConsoleRegisterCommand(capCommandName, sizeof(capCommandName), functionCAP);
    
    return;
}
