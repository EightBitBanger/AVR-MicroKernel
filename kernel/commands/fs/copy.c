#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/copy.h>

uint8_t msgFileCopied[]   = "File copied";

void functionCOPY(uint8_t* param, uint8_t param_length) {
    
    uint8_t sourceFilename[FILE_NAME_LENGTH];
    uint8_t destFilename[FILE_NAME_LENGTH];
    
    uint8_t sourceNameLength = 0;
    uint8_t destNameLength   = 0;
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
        sourceFilename[i] = 0;
        destFilename[i]   = 0;
    }
    
    // Get filenames from params
    uint8_t filename_index = 0;
    uint8_t index = 0;
    
    for (uint8_t i=0; i < param_length; i++) {
        
        if (filename_index == 0) {
            
            sourceFilename[i] = param[i];
            
            if (param[i] == ' ') {
                
                filename_index = 1;
                
                sourceNameLength = i;
                
                continue;
            }
            
        } else {
            
            if (destFilename[index] == ' ') 
                break;
            
            destFilename[index] = param[i];
            
            destNameLength = i;
            
            index++;
        }
        
    }
    
    print(sourceFilename, sourceNameLength);
    printLn();
    
    print(destFilename, destNameLength);
    printLn();
    
    //print(msgFileCopied, sizeof(msgFileCopied));
    //printLn();
    
    return;
}

void registerCommandCOPY(void) {
    
    uint8_t copyCommandName[] = "copy";
    
    ConsoleRegisterCommand(copyCommandName, sizeof(copyCommandName), functionCOPY);
    
    return;
}
