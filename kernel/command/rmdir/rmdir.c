#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/rmdir/rmdir.h>

void functionRMDIR(uint8_t* param, uint8_t param_length) {
    
    if (fsDirectoryDelete(param, param_length-1) != 0) {
        
        uint8_t msgDirRemoved[]   = "Directory removed";
        
        print(msgDirRemoved, sizeof(msgDirRemoved));
        printLn();
        
    } else {
        
        uint8_t msgDirNotFound[]  = "Directory not found";
        
        print(msgDirNotFound, sizeof(msgDirNotFound));
        printLn();
        
        return;
    }
    
    return;
}

void registerCommandRMDIR(void) {
    
    uint8_t rmdirCommandName[] = "rmdir";
    
    ConsoleRegisterCommand(rmdirCommandName, sizeof(rmdirCommandName), functionRMDIR);
    
    return;
}
