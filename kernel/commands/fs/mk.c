#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/mk.h>

void functionMK(uint8_t* param, uint8_t param_length) {
    
    uint16_t filesize = 100;
    
    if (fsFileCreate(param, param_length, filesize) == 1) {
        
        uint8_t msgTest[] = "1 File(s) created";
        print(msgTest, sizeof(msgTest));
        printLn();
        
    } else {
        
        uint8_t msgTest[] = "No space available";
        print(msgTest, sizeof(msgTest));
        printLn();
        
    }
    
    return;
}

void registerCommandMK(void) {
    
    uint8_t mkCommandName[] = "mk";
    
    ConsoleRegisterCommand(mkCommandName, sizeof(mkCommandName), functionMK);
    
    return;
}
