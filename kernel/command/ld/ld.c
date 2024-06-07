#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/ld/ld.h>

void functionLD(uint8_t* param, uint8_t param_length) {
    
    struct Driver driverPtr;
    
    LoadLibrary(param, param_length, &driverPtr);
    
    return;
    
    uint32_t fileSize = fsGetFileSize(param, param_length);
    
    uint8_t fileBuffer[fileSize];
    
    uint8_t index = fsFileOpen(param, param_length);
    
    fsFileReadBin(index, fileBuffer, fileSize);
    
    fsFileClose(index);
    
    
    for (uint32_t i=0; i < fileSize; i++) {
        
        printChar( fileBuffer[i] );
        
    }
    
    return;
}



void registerCommandLD(void) {
    
    uint8_t ldCommandName[] = "ld";
    
    ConsoleRegisterCommand(ldCommandName, sizeof(ldCommandName), functionLD);
    
    return;
}
