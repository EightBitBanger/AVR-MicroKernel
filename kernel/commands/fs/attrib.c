#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/attrib.h>

uint8_t msgFileNotFound[]     = "File not found";
uint8_t msgAttributeSet[]     = "File attribute set";
uint8_t msgAttributeNotSet[]  = "Error setting attribute";

void functionATTRIB(uint8_t* param, uint8_t param_length) {
    
    // Get the file name
    uint8_t filename[10];
    uint8_t filenameLength = 0;
    
    for (uint8_t i=0; i < param_length; i++) {
        
        filenameLength = i + 2;
        
        if (param[i] == ' ') 
            break;
        
        filename[i] = param[i];
        
    }
    
    // Check file exists
    if (fsFileExists(param, param_length) == 0) {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
        return;
    }
    
    //
    // Determine the applied attributes
    struct FSAttribute attribute;
    attribute.executable  = 1;
    attribute.readable    = 1;
    attribute.writeable   = 1;
    
    for (uint8_t i=0; i < 6; i++) {
        
        filenameLength = i + 2;
        
        if (param[i] == ' ') 
            break;
        
        filename[i] = param[i];
        
    }
    
    // Apply file attributes
    if (fsSetFileAttributes(filename, filenameLength-1, &attribute) == 0) {
        
        print(msgAttributeNotSet, sizeof(msgAttributeNotSet));
        printLn();
        
        return;
    }
    
    print(msgAttributeSet, sizeof(msgAttributeSet));
    printLn();
    
    return;
    
    /*
    
    uint8_t stringFileSize[10];
    uint8_t place = int_to_string(filesize, stringFileSize);
    
    print(stringFileSize, place + 1);
    
    printSpace(1);
    
    print(msgInBytes, sizeof(msgInBytes));
    printLn();
    
    
    // Check if the file already exists
    if (fsFileExists(param, param_length) != 0) {
        
        print(msgAlreadyExists, sizeof(msgAlreadyExists));
        printLn();
        
        return;
    }
    
    if (fsFileCreate(param, param_length, filesize) == 0) {
        
        print(msgNoSpace, sizeof(msgNoSpace));
        printLn();
        
    } else {
        
        print(msgFileCreated, sizeof(msgFileCreated));
        printLn();
        
    }
    
    */
    
    return;
}

void registerCommandATTRIB(void) {
    
    uint8_t attribCommandName[] = "attrib";
    
    ConsoleRegisterCommand(attribCommandName, sizeof(attribCommandName), functionATTRIB);
    
    return;
}
