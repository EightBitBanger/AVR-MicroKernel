#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/cd/cd.h>

struct DirectoryName {
    
    uint8_t name[10];
    
};


struct DirectoryName directoryStack[8];

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
    
    // Check internal storage switch
    if ((param[0] == '/') & (param[1] == ':')) {
        
        fsSetCurrentDevice( 0xff );
        fsSetRootDirectory('/');
        
        uint8_t PromptBase[] = "/>";
        ConsoleSetPrompt(PromptBase, sizeof(PromptBase));
        
        fsClearWorkingDirectory();
        
        fsSetDirectoryStack(0);
        
        return;
    }
    
    
    //
    // Drop down the parent directory
    //
    
    if ((param[0] == '.') & (param[1] == '.') & (param[2] == ' ')) {
        
        uint8_t directoryStackPtr = fsGetDirectoryStack();
        
        if (directoryStackPtr > 1) {
            
            // Drop to parent directory
            directoryStackPtr--;
            
            fsSetDirectoryStack(directoryStackPtr);
            
            uint8_t filename[20];
            uint8_t filenameLength = 0;
            
            for (uint8_t n=0; n < 10; n++) {
                
                filename[n] = directoryStack[directoryStackPtr].name[n];
                
                filenameLength = n + 1;
                
                if (filename[n] == ' ') 
                    break;
            }
            
            fsSetWorkingDirectory(filename, filenameLength-1);
            
            uint8_t PromptDir[20];
            
            if (fsGetRootDirectory() == '/') {
                
                for (uint8_t n=0; n < 10; n++) 
                    directoryStack[directoryStackPtr].name[n] = ' ';
                
                for (uint8_t n=0; n < filenameLength - 1; n++) {
                    directoryStack[directoryStackPtr].name[n] = filename[n];
                    
                    PromptDir[n + 1] = filename[n];
                }
                
                PromptDir[filenameLength] = '>';
                
                PromptDir[0] = '/';
                
                ConsoleSetPrompt(PromptDir, filenameLength + 2);
                
            }
            
        } else {
            
            // Drop to root directory
            
            directoryStackPtr--;
            
            fsSetDirectoryStack(directoryStackPtr);
            
            uint8_t PromptRoot[] = " >";
            
            PromptRoot[0] = fsGetRootDirectory();
            
            ConsoleSetPrompt(PromptRoot, 3);
            
            fsClearWorkingDirectory();
            
        }
        
        return;
    }
    
    
    //
    // Change to system root
    //
    
    if ((param[0] == '/') & (param[1] == ' ')) {
        
        fsClearWorkingDirectory();
        
        fsSetDirectoryStack(0);
        
        uint8_t PromptRoot[] = " >";
        
        PromptRoot[0] = fsGetRootDirectory();
        
        ConsoleSetPrompt(PromptRoot, 3);
        
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
                
                fsSetWorkingDirectory(param, param_length-1);
                
                uint8_t PromptDir[20];
                
                if (fsGetRootDirectory() == '/') {
                    
                    // Root directory
                    uint8_t directoryStackPtr = fsGetDirectoryStack() + 1;
                    fsSetDirectoryStack(directoryStackPtr);
                    
                    for (uint8_t n=0; n < 10; n++) 
                        directoryStack[directoryStackPtr].name[n] = ' ';
                    
                    for (uint8_t n=0; n < param_length - 1; n++) {
                        directoryStack[directoryStackPtr].name[n] = param[n];
                        
                        PromptDir[n + 1] = param[n];
                    }
                    
                    PromptDir[param_length] = '>';
                    
                    PromptDir[0] = '/';
                    
                    ConsoleSetPrompt(PromptDir, param_length + 2);
                    
                } else {
                    
                    // Device letter
                    
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
