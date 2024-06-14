#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/cd/cd.h>

struct DirectoryName {
    
    uint8_t name[10];
    
};


struct DirectoryName directoryStack[16];

extern uint8_t fs_directory_stack_ptr;


void functionCD(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgDeviceError[]        = "Device not ready";
    uint8_t msgDirectoryNotFound[]  = "Directory not found";
    uint8_t msgNotDirectory[]       = "Not a directory";
    
    
    //
    // Change Device
    //
    
    // The letter represents the hardware address 
    // offset pointing to the device on the system bus
    
    uint8_t deviceLetter = param[0];
    uppercase(&deviceLetter);
    
    if ((deviceLetter >= 'A') & (deviceLetter <= 'Z') & (param[1] == ':')) {
        
        fsSetDeviceLetter( deviceLetter );
        fsSetRootDirectory( deviceLetter );
        
        uint8_t consolePrompt[2];
        consolePrompt[0] = deviceLetter;
        consolePrompt[1] = '>';
        
        ConsoleSetPrompt(consolePrompt, 3);
        
        fsClearWorkingDirectory();
        fs_directory_stack_ptr = 0;
        
        return;
    }
    
    
    //
    // Change Directory
    //
    
    if ((deviceLetter >= 'A') & (deviceLetter <= 'Z')) {
        
        if (fsCheckDeviceReady() == 0) {
            
            print(msgDeviceError, sizeof(msgDeviceError));
            printLn();
            
            return;
        }
        
        // Check for sub directory of the working directory
        if (fsCheckWorkingDirectory() == 0) {
            
            // Check root for a directory
            
            // Check directory is not claimed by a directory
            if (fsDirectoryGetFlag(param, param_length) != 0) {
                
                print(msgDirectoryNotFound, sizeof(msgDirectoryNotFound));
                printLn();
                
                return;
            }
            
        }
        
        // Check is NOT a directory
        if (fsCheckIsDirectory(param, param_length-1) == 0) {
            
            print(msgNotDirectory, sizeof(msgNotDirectory));
            printLn();
            
            return;
        }
        
        if (fsSetWorkingDirectory(param, param_length) == 0) {
            
            print(msgDirectoryNotFound, sizeof(msgDirectoryNotFound));
            printLn();
            
            return;
        }
        
        fs_directory_stack_ptr++;
        
        // Add the directory to the directory stack
        for (uint8_t n=0; n < 10; n++) 
            directoryStack[fs_directory_stack_ptr].name[n] = ' ';
        
        // Prompt for device letters
        
        for (uint8_t n=0; n < param_length + 1; n++) 
            directoryStack[fs_directory_stack_ptr].name[n] = param[n];
        
        uint8_t PromptDir[20];
        
        // Device letter
        for (uint8_t i=0; i < param_length + 2; i++) 
            PromptDir[i + 2] = param[i];
        
        PromptDir[param_length + 1] = '>';
        
        PromptDir[0] = fsGetRootDirectory();
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
            
            // Set the parent directory
            fsSetWorkingDirectory(filename, filenameLength - 1);
            
            uint8_t PromptDir[20];
            
            for (uint8_t n=0; n < 10; n++) 
                directoryStack[fs_directory_stack_ptr].name[n] = ' ';
            
            // Prompt for device letters
            
            for (uint8_t n=0; n < filenameLength - 1; n++) {
                directoryStack[fs_directory_stack_ptr].name[n] = filename[n];
                
                PromptDir[n + 2] = filename[n];
            }
            
            PromptDir[filenameLength] = '>';
            
            PromptDir[0] = fsGetRootDirectory();
            PromptDir[1] = '/';
            
            ConsoleSetPrompt(PromptDir, filenameLength + 2);
            
        } else {
            
            // Drop to the root directory
            
            fs_directory_stack_ptr = 0;
            
            uint8_t PromptRoot[] = " >";
            
            PromptRoot[0] = fsGetRootDirectory();
            
            ConsoleSetPrompt(PromptRoot, 3);
            
            fsClearWorkingDirectory();
            
        }
        
        return;
    }
    
    
    
    //
    // Display the full path
    //
    
    if ((param[0] == '.') & (param[1] == ' ')) {
        
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
    
    
    
    //
    // Drop down to the root
    //
    
    if ((param[0] == '/') & (param[1] == ' ')) {
        
        // Drop to the root directory
        
        fs_directory_stack_ptr = 0;
        
        uint8_t PromptRoot[] = " >";
        
        PromptRoot[0] = fsGetRootDirectory();
        
        ConsoleSetPrompt(PromptRoot, 3);
        
        fsClearWorkingDirectory();
        
        return;
    }
    
    return;
}

void registerCommandCD(void) {
    
    uint8_t cdCommandName[] = "cd";
    
    ConsoleRegisterCommand(cdCommandName, sizeof(cdCommandName), functionCD);
    
    return;
}
