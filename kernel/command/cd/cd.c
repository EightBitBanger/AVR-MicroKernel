#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/cd/cd.h>


void functionCD(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgDeviceError[]        = "Device not ready";
    uint8_t msgDirectoryNotFound[]  = "Directory not found";
    uint8_t msgNotDirectory[]       = "Not a directory";
    
    //
    // Change device letter
    //
    
    // The letter represents the hardware address 
    // offset pointing to the device on the system bus
    
    if ((param[0] >= 'a') & (param[0] <= 'z') & (param[1] == ':')) {
        
        uint8_t deviceLetter = param[0] - 'a';
        
        fsSetCurrentDevice( deviceLetter );
        uppercase(&param[0]);
        
        uint8_t consolePrompt[2];
        consolePrompt[0] = param[0];
        consolePrompt[1] = '>';
        
        ConsoleSetPrompt(consolePrompt, 3);
        
        fsSetRootDirectory(param[0]);
        
        fsClearWorkingDirectory();
        
        return;
    }
    
    
    //
    // Drop down the parent directory
    //
    
    if ((param[0] == '.') & (param[1] == '.') & (param[2] == ' ')) {
        
        uint8_t PromptRoot[] = " >";
        
        PromptRoot[0] = fsGetRootDirectory();
        
        ConsoleSetPrompt(PromptRoot, 3);
        
        fsClearWorkingDirectory();
        
        return;
    }
    
    
    //
    // Change to system root
    //
    
    if ((param[0] == '/') & ((param[1] == ' ') | (param[1] == ':'))) {
        
        fsSetCurrentDevice( 0xff );
        fsSetRootDirectory('/');
        
        uint8_t PromptBase[] = "/>";
        ConsoleSetPrompt(PromptBase, sizeof(PromptBase));
        
        fsClearWorkingDirectory();
        
        return;
    }
    
    
    //
    // Change to a directory
    //
    
    if ((param[0] >= 'a') & (param[0] <= 'z')) {
        
        if (fsFileExists(param, param_length-1) != 0) {
            
            struct FSAttribute attribute;
            fsGetFileAttributes(param, param_length-1, &attribute);
            
            // Check directory attribute
            if (attribute.type == 'd') {
                
                // Check device
                
                if (fsCheckDeviceReady() == 0) {
                    
                    print(msgDeviceError, sizeof(msgDeviceError));
                    printLn();
                    
                    return;
                }
                
                uint8_t PromptDir[20];
                
                fsSetWorkingDirectory(param, param_length);
                
                // Check root directory preamble
                if (fsGetRootDirectory() == '/') {
                    
                    for (uint8_t i=0; i < param_length + 1; i++) 
                        PromptDir[i + 1] = param[i];
                    
                    PromptDir[param_length] = '>';
                    
                    PromptDir[0] = '/';
                    
                    ConsoleSetPrompt(PromptDir, param_length + 2);
                    
                } else {
                    
                    for (uint8_t i=0; i < param_length + 1; i++) 
                        PromptDir[i + 2] = param[i];
                    
                    PromptDir[param_length + 1] = '>';
                    
                    PromptDir[0] = fsGetRootDirectory();
                    PromptDir[1] = '/';
                    
                    ConsoleSetPrompt(PromptDir, param_length + 3);
                    
                }
                
            } else {
                
                print(msgNotDirectory, sizeof(msgNotDirectory));
                printLn();
                
            }
            
        } else {
            
            print(msgDirectoryNotFound, sizeof(msgDirectoryNotFound));
            printLn();
            
        }
        
        return;
    }
    
    return;
}

void registerCommandCD(void) {
    
    uint8_t cdCommandName[] = "cd";
    
    ConsoleRegisterCommand(cdCommandName, sizeof(cdCommandName), functionCD);
    
    return;
}
