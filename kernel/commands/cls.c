#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/cls.h>

void functionCLS(uint8_t* param, uint8_t param_length) {
    
    uint8_t displayDriverName[] = "display";
    struct Driver* displayDriver = GetDriverByName(displayDriverName, sizeof(displayDriverName));
    
    // Clear display frame buffer
    displayDriver->write( 165, 0x01 );
    
    _delay_ms(40);
    
    ConsoleSetCursor(0, 0);
    
    return;
}

void registerCommandCLS(void) {
    
    uint8_t clsCommandName[] = "cls";
    
    ConsoleRegisterCommand(clsCommandName, sizeof(clsCommandName), functionCLS);
    
    return;
}
