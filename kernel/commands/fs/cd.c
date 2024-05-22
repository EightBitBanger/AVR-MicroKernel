#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/cap.h>

uint8_t msgDeviceError[] = "Device not ready";


void functionCD(uint8_t* param, uint8_t param_length) {
    
    uint8_t deviceLocated = 0;
    
    if ((param[0] >= 'a') & (param[0] <= 'z')) {
        
        fsSetCurrentDevice( param[0] - 'a' );
        
        deviceLocated = 1;
    }
    
    if ((param[0] == 'r') & (param[1] == 'o') & (param[2] == 'o') & (param[3] == 't')) {
        
        fsSetCurrentDevice( param[0] - 'A' );
        
        deviceLocated = 1;
    }
    
    if (deviceLocated == 0) {
        
        print(msgDeviceError, sizeof(msgDeviceError));
        printLn();
        
        return;
    }
    
    uint8_t consolePrompt[2];
    consolePrompt[0] = param[0];
    consolePrompt[1] = '>';
    
    ConsoleSetPrompt(consolePrompt, 3);
    
    return;
}

void registerCommandCD(void) {
    
    uint8_t cdCommandName[] = "cd";
    
    ConsoleRegisterCommand(cdCommandName, sizeof(cdCommandName), functionCD);
    
    return;
}
