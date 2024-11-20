#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/cd/cd.h>

void functionCD(uint8_t* param, uint8_t param_length) {
    
    if (param_length > FILE_NAME_LENGTH) 
        param_length = FILE_NAME_LENGTH;
    
    uint8_t msgDirectoryNotFound[]  = "Directory not found";
    
    uint8_t deviceLetter = param[0];
    uppercase(&deviceLetter);
    
    
    //
    // Drop down the parent directory
    //
    
    if ((param[0] == '.') & (param[1] == '.') & (param[2] == ' ')) {
        
        if (fsWorkingDirectoryGetStack() > 1) {
            
            uint8_t promptLength = 0;
            uint8_t PromptDir[20] = {fsDeviceGetRoot(), '/'};
            
            if (fsWorkingDirectorySetToParent() == 1) {
                
                uint8_t directoryName[FILE_NAME_LENGTH];
                
                for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
                    directoryName[i] = ' ';
                
                fsFileGetName(fsWorkingDirectoryGetAddress(), directoryName);
                
                for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) {
                    
                    PromptDir[n + 2] = directoryName[n];
                    
                    if (PromptDir[n + 2] == ' ') {
                        
                        promptLength = n + 1;
                        break;
                    }
                    
                }
                
                PromptDir[promptLength + 1] = '>';
                
                ConsoleSetPrompt(PromptDir, promptLength + 3);
            }
            
        } else {
            
            uint8_t PromptRoot[] = {fsDeviceGetRoot(), '>'};
            ConsoleSetPrompt(PromptRoot, sizeof(PromptRoot)+1);
            
            fsWorkingDirectoryClear();
            
        }
        
        return;
    }
    
    
    
    //
    // Change Device
    //
    
    // The letter represents the hardware address 
    // offset pointing to the device on the system bus
    
    if ((deviceLetter >= 'A') & (deviceLetter <= 'Z') & (param[1] == ':')) {
        
        fsDeviceSetRoot( deviceLetter );
        
        uint8_t consolePrompt[] = {deviceLetter, '>'};
        
        ConsoleSetPrompt(consolePrompt, sizeof(consolePrompt)+1);
        
        fsWorkingDirectoryClear();
        
        return;
    }
    
    
    //
    // Change Directory
    //
    
    if ((deviceLetter >= 'A') & (deviceLetter <= 'Z')) {
        
        uint32_t directoryAddress = fsDirectoryExists(param, param_length);
        
        if (directoryAddress == 0) {
            
            print(msgDirectoryNotFound, sizeof(msgDirectoryNotFound));
            printLn();
            
            return;
        }
        
        fsWorkingDirectoryChange(param, param_length);
        
        // Prompt for device letters
        
        uint8_t PromptDir[20] = {fsDeviceGetRoot(), '/'};
        
        for (uint8_t i=0; i < param_length + 2; i++) 
            PromptDir[i + 2] = param[i];
        
        PromptDir[param_length + 1] = '>';
        
        ConsoleSetPrompt(PromptDir, param_length + 3);
        
        return;
    }
    
    
    //
    // Drop down to the root
    //
    
    if ((param[0] == '/') & (param[1] == ' ')) {
        
        uint8_t PromptRoot[] = {fsDeviceGetRoot(), '>'};
        ConsoleSetPrompt(PromptRoot, sizeof(PromptRoot)+1);
        
        fsWorkingDirectoryClear();
        
        return;
    }
    
    
    //
    // Display the full path
    //
    
    printChar('/');
    
    uint32_t currentDirectory = fsWorkingDirectoryGetAddress();
    
    if (currentDirectory == 0) 
        return;
    
    uint32_t dirStack[32] = {0};
    uint8_t DirectoryCount = 0;
    
    for (uint8_t i=0; i < fsWorkingDirectoryGetStack(); i++) {
        
        dirStack[i] = currentDirectory;
        
        currentDirectory = fsWorkingDirectoryGetParent();
        
        DirectoryCount = i;
        
        if (currentDirectory == 0) {
            
            dirStack[i + 1] = 0;
            
            break;
        }
        
    }
    
    for (uint8_t i=DirectoryCount; i >= 0; i--) {
        
        if (dirStack[i] == 0) 
            break;
        
        uint8_t directoryName[FILE_NAME_LENGTH];
        
        for (uint8_t a=0; a < FILE_NAME_LENGTH; a++) 
            directoryName[a] = ' ';
        
        fsFileGetName(dirStack[i], directoryName);
        
        for (uint8_t c=0; c < FILE_NAME_LENGTH; c++) {
            
            if (directoryName[c] == ' ') 
                break;
            
            printChar( directoryName[c] );
            
        }
        
        if (i == 0) 
            break;
        
        printChar('/');
        
        continue;
    }
    
    printLn();
    
    return;
}

void registerCommandCD(void) {
    
    uint8_t cdCommandName[] = "cd";
    
    ConsoleRegisterCommand(cdCommandName, sizeof(cdCommandName), functionCD);
    
    return;
}
