#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/attrib/attrib.h>

void functionATTRIB(uint8_t* param, uint8_t param_length) {
    
    /*
    
    uint8_t msgFileNotFound[]    = "File not found";
    uint8_t msgAttributeError[]  = "Error";
    
    // Get the file name
    uint8_t filename[10];
    uint8_t filenameLength = 0;
    
    for (uint8_t i=0; i < param_length; i++) {
        
        filenameLength = i;
        
        if (param[i] == ' ') 
            break;
        
        filename[i] = param[i];
        
    }
    
    fsFileExists();
    
    // Get current file attributes
    struct FSAttribute attributeCurrent;
    if (fsGetFileAttributes(filename, filenameLength, &attributeCurrent) == 0) {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
        return;
    }
    
    
    //
    // Determine the applied attributes
    struct FSAttribute attribute;
    attribute.executable  = attributeCurrent.executable;
    attribute.readable    = attributeCurrent.readable;
    attribute.writeable   = attributeCurrent.writeable;
    attribute.type        = attributeCurrent.type;
    
    // Remove
    if ((param[filenameLength + 1] == '-') & (param[filenameLength + 2] == 'x')) attribute.executable = ' ';
    if ((param[filenameLength + 1] == '-') & (param[filenameLength + 2] == 'r')) attribute.readable   = ' ';
    if ((param[filenameLength + 1] == '-') & (param[filenameLength + 2] == 'w')) attribute.writeable  = ' ';
    
    // Add
    if ((param[filenameLength + 1] == '+') & (param[filenameLength + 2] == 'x')) attribute.executable = 'x';
    if ((param[filenameLength + 1] == '+') & (param[filenameLength + 2] == 'r')) attribute.readable   = 'r';
    if ((param[filenameLength + 1] == '+') & (param[filenameLength + 2] == 'w')) attribute.writeable  = 'w';
    
    // Apply file attributes if any changes are made
    uint8_t isChanged = 0;
    if (attribute.executable != attributeCurrent.executable) isChanged = 1;
    if (attribute.readable   != attributeCurrent.readable)   isChanged = 1;
    if (attribute.writeable  != attributeCurrent.writeable)  isChanged = 1;
    if (attribute.type       != attributeCurrent.type)       isChanged = 1;
    
    // Cannot make directories executable...
    if ((attributeCurrent.type == 'd') & (attribute.type != 'd')) {
        
        print(msgAttributeError, sizeof(msgAttributeError));
        printLn();
        
        return;
    }
    
    // Cannot make directories executable...
    if ((attributeCurrent.type == 'd') & (attribute.executable != ' ')) {
        
        print(msgAttributeError, sizeof(msgAttributeError));
        printLn();
        
        return;
    }
    
    // Update changes
    if (isChanged == 1) {
        
        if (fsSetFileAttributes(filename, filenameLength, &attribute) == 0) {
            
            print(msgAttributeError, sizeof(msgAttributeError));
            printLn();
            
            return;
        }
        
    }
    
    if (attribute.executable != 0) {printChar( attribute.executable );} else {printSpace(1);}
    if (attribute.readable   != 0) {printChar( attribute.readable   );} else {printSpace(1);}
    if (attribute.writeable  != 0) {printChar( attribute.writeable  );} else {printSpace(1);}
    if (attribute.type       != 0) {printChar( attribute.type       );} else {printSpace(1);}
    
    printSpace(1);
    print(param, param_length);
    
    printLn();
    
    */
    
    return;
}

void registerCommandATTRIB(void) {
    
    uint8_t attribCommandName[] = "attrib";
    
    ConsoleRegisterCommand(attribCommandName, sizeof(attribCommandName), functionATTRIB);
    
    return;
}
