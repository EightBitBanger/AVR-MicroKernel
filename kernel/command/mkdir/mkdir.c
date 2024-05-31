#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/mkdir/mkdir.h>

void functionMKDIR(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgFileCreated[]   = "Directory created";
    uint8_t msgNoSpace[]       = "No space available";
    uint8_t msgAlreadyExists[] = "Already exists";
    uint8_t msgBadName[]       = "Invalid name";
    
    if (param[0] == ' ') {
        print(msgBadName, sizeof(msgBadName));
        printLn();
        return;
    }
    
    // Check if the file already exists
    if (fsFileExists(param, param_length) != 0) {
        
        print(msgAlreadyExists, sizeof(msgAlreadyExists));
        printLn();
        
        return;
    }
    
    if (fsDirectoryCreate(param, param_length, 20) == 0) {
        
        print(msgNoSpace, sizeof(msgNoSpace));
        printLn();
        
    } else {
        
        print(msgFileCreated, sizeof(msgFileCreated));
        printLn();
        
    }
    
    return;
}

void registerCommandMKDIR(void) {
    
    uint8_t mkdirCommandName[] = "mkdir";
    
    ConsoleRegisterCommand(mkdirCommandName, sizeof(mkdirCommandName), functionMKDIR);
    
    return;
}
