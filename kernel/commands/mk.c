#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/mk.h>

void functionMK(uint8_t* param, uint8_t param_length) {
    
    
    fsFileCreate(param, param_length, 20);
    
    
    return;
}

void registerCommandMK(void) {
    
    uint8_t mkCommandName[] = "mk";
    
    ConsoleRegisterCommand(mkCommandName, sizeof(mkCommandName), functionMK);
    
    return;
}
