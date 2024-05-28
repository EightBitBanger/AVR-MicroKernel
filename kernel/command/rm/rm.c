#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/rm/rm.h>

void functionRM(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgFileNotFound[]      = "File not found";
    uint8_t msgFileAccessDenied[]  = "Access denied";
    uint8_t msgFileRemoved[]       = "File removed";
    
    
    if (fsFileExists(param, param_length-1) == 0) {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
        return;
    }
    
    // Check directory attribute
    struct FSAttribute attribute;
    if (fsGetFileAttributes(param, param_length-1, &attribute) == 0) {
        
        print(msgFileAccessDenied, sizeof(msgFileAccessDenied));
        printLn();
        
        return;
    }
    
    if (attribute.type == 'd') {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
        return;
    }
    
    if (fsFileDelete(param, param_length-1) == 1) {
        
        print(msgFileRemoved, sizeof(msgFileRemoved));
        printLn();
        
    } else {
        
        print(msgFileAccessDenied, sizeof(msgFileAccessDenied));
        printLn();
        
    }
    
    return;
}

void registerCommandRM(void) {
    
    uint8_t rmCommandName[] = "rm";
    
    ConsoleRegisterCommand(rmCommandName, sizeof(rmCommandName), functionRM);
    
    return;
}
