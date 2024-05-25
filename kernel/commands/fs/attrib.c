#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/attrib.h>

void functionATTRIB(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgFileNotFound[]     = "File not found";
    uint8_t msgAttributeNotSet[]  = "Error setting attribute";
    
    // Get the file name
    uint8_t filename[10];
    uint8_t filenameLength = 0;
    
    for (uint8_t i=0; i < param_length; i++) {
        
        filenameLength = i;
        
        if (param[i] == ' ') 
            break;
        
        filename[i] = param[i];
        
    }
    
    // Check file exists
    if (fsFileExists(filename, filenameLength) == 0) {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
        return;
    }
    
    // Get current file attributes
    struct FSAttribute attributeCurrent;
    fsGetFileAttributes(filename, filenameLength, &attributeCurrent);
    
    //
    // Determine the applied attributes
    struct FSAttribute attribute;
    attribute.executable  = attributeCurrent.executable;
    attribute.readable    = attributeCurrent.readable;
    attribute.writeable   = attributeCurrent.writeable;
    
    if ((param[filenameLength+1] == '-') & (param[filenameLength + 2] == 'x')) attribute.executable = 0;
    if ((param[filenameLength+1] == '-') & (param[filenameLength + 2] == 'r')) attribute.readable   = 0;
    if ((param[filenameLength+1] == '-') & (param[filenameLength + 2] == 'w')) attribute.writeable  = 0;
    
    if (param[filenameLength+1] == 'x') attribute.executable = 1;
    if (param[filenameLength+1] == 'r') attribute.readable   = 1;
    if (param[filenameLength+1] == 'w') attribute.writeable  = 1;
    
    // Apply file attributes if any changes are made
    uint8_t isChanged = 0;
    if (attribute.executable != attributeCurrent.executable) isChanged = 1;
    if (attribute.readable   != attributeCurrent.readable)   isChanged = 1;
    if (attribute.writeable  != attributeCurrent.writeable)  isChanged = 1;
    
    if (isChanged == 1) {
        
        if (fsSetFileAttributes(filename, filenameLength-1, &attribute) == 0) {
            
            print(msgAttributeNotSet, sizeof(msgAttributeNotSet));
            printLn();
            
            return;
        }
        
    }
    
    if (attribute.executable == 1) {printChar('x');} else {printSpace(1);}
    if (attribute.readable   == 1) {printChar('r');} else {printSpace(1);}
    if (attribute.writeable  == 1) {printChar('w');} else {printSpace(1);}
    
    printLn();
    
    return;
}

void registerCommandATTRIB(void) {
    
    uint8_t attribCommandName[] = "attrib";
    
    ConsoleRegisterCommand(attribCommandName, sizeof(attribCommandName), functionATTRIB);
    
    return;
}
