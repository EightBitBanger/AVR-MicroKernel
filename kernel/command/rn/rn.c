#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/rn/rn.h>

void functionRN(uint8_t* param, uint8_t param_length) {
    
    uint8_t sourceName[10];
    uint8_t sourceNameLen = 0;
    
    for (uint8_t i=0; i < param_length; i++) {
        
        sourceName[i] = param[i];
        
        sourceNameLen++;
        
        if (param[i] == ' ') 
            break;
        
    }
    
    uint8_t targetName[10];
    uint8_t targetNameLen = 0;
    
    for (uint8_t i=0; i < param_length; i++) {
        
        targetName[i] = param[i + sourceNameLen];
        
        targetNameLen++;
        
        if (i > sourceNameLen + 1) 
            break;
        
    }
    
    fsFileRename(sourceName, sourceNameLen, targetName, targetNameLen);
    
    return;
}

void registerCommandRN(void) {
    
    uint8_t rnCommandName[] = "rn";
    
    ConsoleRegisterCommand(rnCommandName, sizeof(rnCommandName), functionRN);
    
    return;
}
