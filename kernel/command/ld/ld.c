#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/ld/ld.h>

void functionLD(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgDriverLoaded[]      = "Driver loaded";
    uint8_t msgCannotLoadDriver[]  = "Already loaded";
    uint8_t msgDriverNotFound[]    = "File not found";
    
    if (fsFileExists(param, param_length) == 0) {
        print(msgDriverNotFound, sizeof(msgDriverNotFound));
        printLn();
        return;
    }
    
    if (LoadLibrary(param, param_length) == 1) {
        print(msgDriverLoaded, sizeof(msgDriverLoaded));
        printLn();
    } else {
        print(msgCannotLoadDriver, sizeof(msgCannotLoadDriver));
        printLn();
    }
    
    return;
}



void registerCommandLD(void) {
    
    uint8_t ldCommandName[] = "ld";
    
    ConsoleRegisterCommand(ldCommandName, sizeof(ldCommandName), functionLD);
    
    return;
}
