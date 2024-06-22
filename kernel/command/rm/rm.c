#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/rm/rm.h>

void functionRM(uint8_t* param, uint8_t param_length) {
    
    /*
    
    uint8_t msgFileNotFound[]      = "File not found";
    uint8_t msgAccessDenied[]      = "Access denied";
    uint8_t msgFileRemoved[]       = "File removed";
    uint8_t msgBadName[]           = "Invalid filename";
    
    if (param_length < 1) {
        print(msgBadName, sizeof(msgBadName));
        printLn();
        return;
    }
    
    // Check directory attribute
    struct FSAttribute attribute;
    if (fsFileGetAttributes(param, param_length-1, &attribute) == 0) {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
        return;
    }
    
    // Ignore directories
    if (attribute.type == 'd') {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
        return;
    }
    
    // Check read only
    if (attribute.writeable != 'w') {
        
        print(msgAccessDenied, sizeof(msgAccessDenied));
        printLn();
        
        return;
    }
    
    if (fsFileDelete(param, param_length-1) == 1) {
        
        print(msgFileRemoved, sizeof(msgFileRemoved));
        printLn();
        
    } else {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
    }
    
    */
    
    return;
}

void registerCommandRM(void) {
    
    uint8_t rmCommandName[] = "rm";
    
    ConsoleRegisterCommand(rmCommandName, sizeof(rmCommandName), functionRM);
    
    return;
}
