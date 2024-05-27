#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/cd/cd.h>


void functionCD(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgDeviceError[]        = "Device not ready";
    uint8_t msgDirectoryNotFound[]  = "Directory not found";
    
    uint8_t deviceLocated = 0;
    
    //
    // Change device letter
    
    if (((param[0] >= 'a') & (param[0] <= 'z')) & 
        ((param[1] == ' ') | (param[1] == ':'))) {
        
        fsSetCurrentDevice( param[0] - 'a' );
        
        uppercase(&param[0]);
        
        uint8_t consolePrompt[2];
        consolePrompt[0] = param[0];
        consolePrompt[1] = '>';
        
        ConsoleSetPrompt(consolePrompt, 3);
        
        deviceLocated = 1;
        
    } else {
        
        //
        // Change to a directory
        
        if ((param[0] >= 'a') & (param[0] <= 'z')) {
            
            if (fsFileExists(param, param_length) != 0) {
                
                uint8_t PromptRoot[param_length + 1];
                
                for (uint8_t i=0; i < param_length; i++)
                    PromptRoot[i] = param[i];
                
                PromptRoot[param_length - 1] = '>';
                
                ConsoleSetPrompt(PromptRoot, sizeof(PromptRoot));
                
            } else {
                
                print(msgDirectoryNotFound, sizeof(msgDirectoryNotFound));
                printLn();
                
            }
            
            deviceLocated = 1;
            
        }
        
    }
    
    //
    // Change root device
    
    if ((param[0] == '/') & ((param[1] == ' ') | (param[1] == ':'))) {
        
        fsSetCurrentDevice( 0xff );
        
        uint8_t PromptRoot[] = "/>";
        ConsoleSetPrompt(PromptRoot, sizeof(PromptRoot));
        
        deviceLocated = 1;
    }
    
    if (deviceLocated == 0) {
        
        print(msgDeviceError, sizeof(msgDeviceError));
        printLn();
        
        return;
    }
    
    return;
}

void registerCommandCD(void) {
    
    uint8_t cdCommandName[] = "cd";
    
    ConsoleRegisterCommand(cdCommandName, sizeof(cdCommandName), functionCD);
    
    return;
}
