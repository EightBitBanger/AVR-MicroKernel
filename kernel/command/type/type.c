#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/type/type.h>

void functionType(uint8_t* param, uint8_t param_length) {
    
    uint32_t fileAddress = fsFileExists(param, param_length);
    
    if (fileAddress == 0) 
        return;
    
    fsFileOpen(fileAddress);
    
    uint32_t fileSize = fsFileGetSize();
    
    if (fileSize == 0) 
        return;
    
    uint8_t fileBuffer[fileSize];
    
    fsFileRead(fileBuffer, fileSize);
    
    fsFileClose();
    
    for (uint32_t i=0; i < fileSize; i++) 
        printChar( fileBuffer[i] );
    
    return;
}



void registerCommandType(void) {
    
    uint8_t typeCommandName[] = "type";
    
    ConsoleRegisterCommand(typeCommandName, sizeof(typeCommandName), functionType);
    
    return;
}
