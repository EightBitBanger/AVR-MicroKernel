#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/rm.h>

void functionRM(uint8_t* param, uint8_t param_length) {
    
    fsFileDelete(param, param_length);
    
    return;
}

void registerCommandRM(void) {
    
    uint8_t rmCommandName[] = "rm";
    
    ConsoleRegisterCommand(rmCommandName, sizeof(rmCommandName), functionRM);
    
    return;
}
