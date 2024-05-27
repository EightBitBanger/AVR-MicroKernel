#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/rm/rm.h>

void functionRM(uint8_t* param, uint8_t param_length) {
    
    if (fsFileDelete(param, param_length) == 1) {
        
        uint8_t msgTest[] = "File removed";
        print(msgTest, sizeof(msgTest));
        printLn();
        
    } else {
        
        uint8_t msgTest[] = "File not found";
        print(msgTest, sizeof(msgTest));
        printLn();
        
    }
    
    return;
}

void registerCommandRM(void) {
    
    uint8_t rmCommandName[] = "rm";
    
    ConsoleRegisterCommand(rmCommandName, sizeof(rmCommandName), functionRM);
    
    return;
}
