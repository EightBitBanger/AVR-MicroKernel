#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/edit.h>

void functionEDIT(uint8_t* param, uint8_t param_length) {
    
    // File state
    uint8_t filename[10];
    uint8_t filename_length = 0;
    
    if (param_length < 3) 
        return;
    
    for (uint8_t i=0; i < param_length+1; i++) {
        if (param[i] == ' ') {
            filename_length = i;
            break;
        }
        
        filename[i] = param[i];
    }
    
    // Cursor state
    uint8_t lastChar = ConsoleGetLastChar();
    uint8_t cursorPos  = 0;
    uint8_t cursorLine = 0;
    
    ConsoleClearScreen();
    
    ConsoleSetCursor(cursorLine, cursorPos);
    
    // Open file
    
    fsFileCreate(filename, filename_length, 100);
    
    
    
    uint8_t isActive = 1;
    while (isActive) {
        
        uint8_t currentChar = ConsoleGetRawChar();
        
        if (lastChar == currentChar) 
            continue;
        
        lastChar = currentChar;
        
        // Escape exit
        if (lastChar == 0x07) {
            
            // TODO: Save/Quit/Cancel menu option
            
            // Quit the application
            isActive = 0;
        }
        
        // Cursor down
        if (lastChar == 0x04) {
            if (cursorLine < 3)
                cursorLine++;
            
        }
        
        // Cursor left
        if (lastChar == 0x05) {
            if (cursorPos > 0) 
                cursorPos--;
            
        }
        // Cursor up
        if (lastChar == 0x03) {
            if (cursorLine > 0) 
                cursorLine--;
            
        }
        
        // Cursor right
        if (lastChar == 0x06) {
            if (cursorPos < 19)
                cursorPos++;
            
        }
        
        
        //
        // Place character
        if (lastChar > 0x19) {
            
            printChar(lastChar);
            
            if (cursorPos < 19) {
                
                cursorPos++;
                
            } else {
                
                cursorPos = 0;
                
                if (cursorLine < 3) 
                    cursorLine++;
                
            }
            
        }
        
        ConsoleSetCursor(cursorLine, cursorPos);
        
        continue;
    }
    
    return;
}

void registerCommandEDIT(void) {
    
    uint8_t editCommandName[] = "edit";
    
    ConsoleRegisterCommand(editCommandName, sizeof(editCommandName), functionEDIT);
    
    return;
}
