#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/format.h>

void functionFORMAT(uint8_t* param, uint8_t param_length) {
    
    fsFormatDevice();
    
    return;
}

void registerCommandFormat(void) {
    
    uint8_t formatCommandName[] = "format";
    
    ConsoleRegisterCommand(formatCommandName, sizeof(formatCommandName), functionFORMAT);
    
    return;
}
