#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/copy.h>

uint8_t msgFileCopied[]       = "File copied";
uint8_t msgSourceNotFound[]   = "Source not found";
uint8_t msgCannotCopyFile[]   = "Cannot be copied";

void functionCOPY(uint8_t* param, uint8_t param_length) {
    
    uint8_t sourceFilename[FILE_NAME_LENGTH];
    uint8_t destFilename[FILE_NAME_LENGTH];
    
    uint8_t sourceNameLength = 0;
    uint8_t destNameLength   = 0;
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
        sourceFilename[i] = ' ';
        destFilename[i]   = ' ';
    }
    
    // Get filenames from params
    uint8_t filename_index = 0;
    uint8_t index = 0;
    
    for (uint8_t i=0; i < param_length; i++) {
        
        if (filename_index == 0) {
            
            sourceFilename[i] = param[i];
            
            if (param[i] == ' ') {
                
                filename_index = 1;
                
                sourceNameLength = i + 1;
                
                continue;
            }
            
        } else {
            
            destFilename[index] = param[i];
            
            destNameLength = i + 1;
            
            if (destFilename[index] == ' ') 
                break;
            
            index++;
            
        }
        
    }
    
    // Check source file exists
    if (fsFileExists(sourceFilename, sourceNameLength) == 0) {
        
        print(msgSourceNotFound, sizeof(msgSourceNotFound));
        printLn();
        
        return;
    }
    
    // Check if the destination exists
    if (fsFileExists(destFilename, destNameLength) == 1) {
        
        print(msgCannotCopyFile, sizeof(msgCannotCopyFile));
        printLn();
        
        return;
    }
    
    print(msgFileCopied, sizeof(msgFileCopied));
    printLn();
    
    return;
}

void registerCommandCOPY(void) {
    
    uint8_t copyCommandName[] = "copy";
    
    ConsoleRegisterCommand(copyCommandName, sizeof(copyCommandName), functionCOPY);
    
    return;
}
