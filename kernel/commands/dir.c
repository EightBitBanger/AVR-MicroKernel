#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/dir.h>

void functionDIR(uint8_t* param, uint8_t param_length) {
    
    fsListDirectory();
    
    return;
}

void registerCommandDIR(void) {
    
    uint8_t dirCommandName[] = "dir";
    
    ConsoleRegisterCommand(dirCommandName, sizeof(dirCommandName), functionDIR);
    
    return;
}
