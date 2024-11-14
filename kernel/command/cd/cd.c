#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/cd/cd.h>

struct Directory { uint8_t name[FILE_NAME_LENGTH]; uint32_t address; };

extern struct Directory directoryStack[WORKNG_DIRECTORY_STACK_SIZE];

extern uint8_t fs_directory_stack_ptr;
extern uint8_t fs_working_directory[FILE_NAME_LENGTH];


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
        
        if (fs_directory_stack_ptr > 1) {
            
            uint8_t promptLength = 0;
            uint8_t PromptDir[20] = {fsDeviceGetRoot(), '/'};
            
            fsWorkingDirectorySetToParent();
            
            for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) {
                
                PromptDir[n + 2] = fs_working_directory[n];
                
                if (PromptDir[n + 2] == ' ') {
                    
                    promptLength = n + 1;
                    break;
                }
                
            }
            
            PromptDir[promptLength + 1] = '>';
            
            ConsoleSetPrompt(PromptDir, promptLength + 3);
            
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
        
        fsDeviceSetLetter( deviceLetter );
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
    
    
    //printInt( fs_directory_stack_ptr );
    
    //printLn();
    
    //return;
    
    
    
    printChar('/');
    
    for (uint8_t i=0; i < fs_directory_stack_ptr; i++) {
        
        for (uint8_t c=0; c < FILE_NAME_LENGTH; c++) {
            
            if (directoryStack[i].name[c] == ' ') 
                break;
            
            printChar( directoryStack[i].name[c] );
            
        }
        
        if (i == fs_directory_stack_ptr) 
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
