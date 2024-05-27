#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/rmdir/rmdir.h>

void functionRMDIR(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgDirRemoved[]   = "Directory removed";
    uint8_t msgDirInvalid[]   = "Not a directory";
    uint8_t msgNotFound[]     = "Not found";
    uint8_t msgBadName[]      = "Invalid name";
    
    if (param[0] == ' ') {
        print(msgBadName, sizeof(msgBadName));
        printLn();
        return;
    }
    
    // Check if the directory does not exist
    if (fsFileExists(param, param_length-1) == 0) {
        
        print(msgNotFound, sizeof(msgNotFound));
        printLn();
        
        return;
    }
    
    // Check directory attribute
    struct FSAttribute attribute;
    fsGetFileAttributes(param, param_length-1, &attribute);
    
    if (attribute.type != 'd') {
        
        print(msgDirInvalid, sizeof(msgDirInvalid));
        printLn();
        
        return;
    }
    
    if (fsFileDelete(param, param_length-1) != 0) {
        
        print(msgDirRemoved, sizeof(msgDirRemoved));
        printLn();
        
    }
    
    return;
}

void registerCommandRMDIR(void) {
    
    uint8_t rmdirCommandName[] = "rmdir";
    
    ConsoleRegisterCommand(rmdirCommandName, sizeof(rmdirCommandName), functionRMDIR);
    
    return;
}
