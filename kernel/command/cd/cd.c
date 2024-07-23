#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/cd/cd.h>

struct Directory { uint8_t name[FILE_NAME_LENGTH]; uint32_t address; };

extern struct Directory directoryStack[WORKNG_DIRECTORY_STACK_SIZE];

extern uint8_t fs_directory_stack_ptr;


void functionCD(uint8_t* param, uint8_t param_length) {
    
    if (param_length > FILE_NAME_LENGTH) 
        param_length = FILE_NAME_LENGTH;
    
    //uint8_t msgDeviceError[]        = "Device not ready";
    uint8_t msgDirectoryNotFound[]  = "Directory not found";
    //uint8_t msgNotDirectory[]       = "Not a directory";
    
    uint8_t deviceLetter = param[0];
    uppercase(&deviceLetter);
    
    
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
        
        fsChangeWorkingDirectory(param, param_length);
        
        fs_directory_stack_ptr++;
        
        // Add the directory to the directory stack
        for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
            directoryStack[fs_directory_stack_ptr].name[n] = ' ';
        for (uint8_t n=0; n < param_length + 1; n++) 
            directoryStack[fs_directory_stack_ptr].name[n] = param[n];
        
        directoryStack[fs_directory_stack_ptr].address = directoryAddress;
        
        
        // Prompt for device letters
        
        uint8_t PromptDir[20];
        for (uint8_t i=0; i < 20; i++) 
            PromptDir[i] = ' ';
        
        for (uint8_t i=0; i < param_length + 2; i++) 
            PromptDir[i + 2] = param[i];
        
        PromptDir[param_length + 1] = '>';
        
        PromptDir[0] = fsGetDeviceRoot();
        PromptDir[1] = '/';
        
        ConsoleSetPrompt(PromptDir, param_length + 3);
        
        return;
    }
    
    
    //
    // Drop down the parent directory
    //
    
    if ((param[0] == '.') & (param[1] == '.') & (param[2] == ' ')) {
        
        if (fs_directory_stack_ptr > 1) {
            
            // Drop to the parent directory
            fs_directory_stack_ptr--;
            
            uint8_t filename[20];
            uint8_t filenameLength = 0;
            
            for (uint8_t n=0; n < 10; n++) {
                
                filename[n] = directoryStack[fs_directory_stack_ptr].name[n];
                
                filenameLength = n + 2;
                
                if (filename[n] == ' ') 
                    break;
            }
            
            fsSetWorkingDirectoryAddress( directoryStack[fs_directory_stack_ptr].address );
            
            // Set the parent directory
            fsChangeWorkingDirectory(filename, filenameLength - 1);
            
            uint8_t PromptDir[20];
            
            for (uint8_t n=0; n < 10; n++) 
                directoryStack[fs_directory_stack_ptr].name[n] = ' ';
            
            // Prompt for device letters
            
            for (uint8_t n=0; n < filenameLength - 1; n++) {
                directoryStack[fs_directory_stack_ptr].name[n] = filename[n];
                
                PromptDir[n + 2] = filename[n];
            }
            
            PromptDir[filenameLength] = '>';
            
            PromptDir[0] = fsGetDeviceRoot();
            PromptDir[1] = '/';
            
            ConsoleSetPrompt(PromptDir, filenameLength + 2);
            
        } else {
            
            // Drop to the root directory
            
            fs_directory_stack_ptr = 0;
            
            uint8_t PromptRoot[] = " >";
            
            PromptRoot[0] = fsGetDeviceRoot();
            
            ConsoleSetPrompt(PromptRoot, 3);
            
            fsClearWorkingDirectory();
            
        }
        
        return;
    }
    
    
    
    //
    // Change Device
    //
    
    // The letter represents the hardware address 
    // offset pointing to the device on the system bus
    
    if ((deviceLetter >= 'A') & (deviceLetter <= 'Z') & (param[1] == ':')) {
        
        fsSetDeviceLetter( deviceLetter );
        fsSetDeviceRoot( deviceLetter );
        
        uint8_t consolePrompt[2];
        consolePrompt[0] = deviceLetter;
        consolePrompt[1] = '>';
        
        ConsoleSetPrompt(consolePrompt, 3);
        
        fsClearWorkingDirectory();
        
        return;
    }
    
    
    //
    // Drop down to the root
    //
    
    if ((param[0] == '/') & (param[1] == ' ')) {
        
        // Drop to the root directory
        
        fs_directory_stack_ptr = 0;
        
        uint8_t PromptRoot[] = " >";
        
        PromptRoot[0] = fsGetDeviceRoot();
        
        ConsoleSetPrompt(PromptRoot, 3);
        
        fsClearWorkingDirectory();
        
        return;
    }
    
    
    //
    // Display the full path
    //
    
    printChar('/');
    
    for (uint8_t i=1; i < fs_directory_stack_ptr + 1; i++) {
        
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
