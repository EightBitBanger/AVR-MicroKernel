#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/cap.h>

void functionCD(uint8_t* param, uint8_t param_length) {
    
    uppercase( &param[0] );
    
    fsSetCurrentDevice( (param[0] - 'A') + 1 );
    
    uint8_t consolePrompt[2];
    consolePrompt[0] = param[0];
    consolePrompt[1] = '>';
    
    ConsoleSetPrompt(consolePrompt, 3);
    
    return;
}

void registerCommandCD(void) {
    
    uint8_t cdCommandName[] = "cd";
    
    ConsoleRegisterCommand(cdCommandName, sizeof(cdCommandName), functionCD);
    
    return;
}
