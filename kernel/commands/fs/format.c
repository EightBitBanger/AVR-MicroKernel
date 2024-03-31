#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/format.h>

void functionFORMAT(uint8_t* param, uint8_t param_length) {
    
    uint32_t deviceCapacity = 0;
    
    if  (param[0] == '8') deviceCapacity = CAPACITY_8K;
    if ((param[0] == '1') & (param[1] == '6')) deviceCapacity = CAPACITY_16K;
    if ((param[0] == '3') & (param[1] == '2')) deviceCapacity = CAPACITY_32K;
    
    if (deviceCapacity != 0) {
        
        fsFormatDevice(deviceCapacity);
        
    } else {
        
        uint8_t errorMessageA[] = "Device capacity not ";
        uint8_t errorMessageB[] = "specified";
        
        print(errorMessageA, sizeof(errorMessageA));
        printLn();
        print(errorMessageB, sizeof(errorMessageB));
        printLn();
        
    }
    
    return;
}

void registerCommandFormat(void) {
    
    uint8_t formatCommandName[] = "format";
    
    ConsoleRegisterCommand(formatCommandName, sizeof(formatCommandName), functionFORMAT);
    
    return;
}
