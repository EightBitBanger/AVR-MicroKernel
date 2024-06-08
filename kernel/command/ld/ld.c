#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/ld/ld.h>

void functionLD(uint8_t* param, uint8_t param_length) {
    
    LoadLibrary(param, param_length);
    
    return;
}



void registerCommandLD(void) {
    
    uint8_t ldCommandName[] = "ld";
    
    ConsoleRegisterCommand(ldCommandName, sizeof(ldCommandName), functionLD);
    
    return;
}
