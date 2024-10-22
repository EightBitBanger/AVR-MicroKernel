#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/mk/mk.h>

uint32_t filesize = 20;

void functionMK(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgInBytes[]        = "bytes";
    uint8_t msgFileCreated[]    = "File created";
    uint8_t msgErrorCreating[]  = "Error creating file";
    uint8_t msgInvalidName[]    = "Invalid filename";
    
    if ((param_length == 0) | 
        (param[0] == ' ')) {
        
        print(msgInvalidName, sizeof(msgInvalidName));
        printLn();
        
        return;
    }
    
    // File size
    if ((param[0] == 's') & (param[1] == 'z')) {
        
        uint8_t fileSizeString[3]  = {' ', ' ', ' '};
        
        if ((param[3] != ' ') & (param[4] != ' ')) {
            
            if (param[5] == ' ') {
                
                // Double digit
                for (uint8_t i=0; i < 2; i++) 
                    fileSizeString[i] = param[3 + i];
                
            } else {
                
                // Triple digit
                for (uint8_t i=0; i < 3; i++) 
                    fileSizeString[i] = param[3 + i];
            }
            
            filesize = string_get_int( &fileSizeString[0] );
            
        }
        
        uint8_t stringFileSize[10];
        uint8_t place = int_to_string(filesize, stringFileSize);
        
        print(stringFileSize, place + 1);
        
        printSpace(1);
        
        print(msgInBytes, sizeof(msgInBytes));
        printLn();
        
        return;
    }
    
    if (fsFileCreate(param, param_length, filesize) == 0) {
        
        print(msgErrorCreating, sizeof(msgErrorCreating));
        printLn();
        
    } else {
        
        print(msgFileCreated, sizeof(msgFileCreated));
        printLn();
        
    }
    
    return;
}

void registerCommandMK(void) {
    
    uint8_t mkCommandName[] = "mk";
    
    ConsoleRegisterCommand(mkCommandName, sizeof(mkCommandName), functionMK);
    
    return;
}
