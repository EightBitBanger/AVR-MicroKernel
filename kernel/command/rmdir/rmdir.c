#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/rmdir/rmdir.h>

void functionRMDIR(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgDirRemoved[]   = "Directory removed";
    uint8_t msgDirNotFound[]  = "Directory not found";
    uint8_t msgInvalidName[]    = "Invalid name";
    
    if ((param_length == 0) | 
        (param[0] == ' ')) {
        
        print(msgInvalidName, sizeof(msgInvalidName));
        printLn();
        
        return;
    }
    
    if (fsDirectoryDelete(param, param_length-1) != 0) {
        
        print(msgDirRemoved, sizeof(msgDirRemoved));
        printLn();
        
    } else {
        
        print(msgDirNotFound, sizeof(msgDirNotFound));
        printLn();
        
    }
    
    return;
}

void registerCommandRMDIR(void) {
    
    uint8_t rmdirCommandName[] = "rmdir";
    
    ConsoleRegisterCommand(rmdirCommandName, sizeof(rmdirCommandName), functionRMDIR);
    
    return;
}
