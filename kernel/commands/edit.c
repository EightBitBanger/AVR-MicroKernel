#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/edit.h>

void functionEDIT(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgFileNotFoundEdittor[] = "File not found";
    uint8_t msgSaved[]               = "Saved";
    uint8_t msgSaveQuit[]            = "(S)ave / (Q)uit?";
    uint8_t msgBytes[]               = "bytes";
    
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
    
    if (is_letter(&filename[0]) == 0) 
        return;
    
    if (filename[0] == ' ') 
        return;
    
    if (filename_length < 1) 
        return;
    
    // Cursor state
    uint8_t lastChar = ConsoleGetLastChar();
    uint8_t cursorPos  = 0;
    uint8_t cursorLine = 0;
    
    // Open file
    uint8_t index = fsFileOpen(filename, filename_length);
    
    uint32_t fileAddressBegin = fsFileExists(filename, filename_length);
    uint32_t fileSize         = fsGetFileSize(filename, filename_length);
    
    if (fileAddressBegin == 0) {
        
        print( msgFileNotFoundEdittor, sizeof(msgFileNotFoundEdittor) );
        printLn();
        
        return;
    }
    
    // Editor buffer
    uint8_t textBuffer[fileSize];
    for (uint16_t i=0; i < fileSize; i++) 
        textBuffer[i] = ' ';
    
    // Null terminate the string
    textBuffer[fileSize] = '\0';
    
    // Load the file
    fsFileRead(index, textBuffer, fileSize);
    
    ConsoleClearScreen();
    
    ConsoleSetCursor(0, 0);
    
    uint8_t isActive = 1;
    while (isActive) {
        
        uint8_t currentChar = ConsoleGetRawChar();
        
        if (lastChar == currentChar) 
            continue;
        
        lastChar = currentChar;
        
        // Escape exit
        if (lastChar == 0x07) {
            
            // Save/Quit/Cancel menu option
            
            ConsoleClearScreen();
            
            ConsoleSetCursor(0, 0);
            
            print(msgSaveQuit, sizeof(msgSaveQuit));
            
            ConsoleSetCursor(2, 0);
            
            print(filename, filename_length + 1);
            
            ConsoleSetCursor(3, 0);
            printInt(fileSize);
            
            print(msgBytes, sizeof(msgBytes));
            
            ConsoleSetBlinkRate(0);
            
            uint8_t answere = 0;
            while (answere == 0) {
                
                currentChar = ConsoleGetRawChar();
                
                if (lastChar == currentChar) 
                    continue;
                
                lastChar = currentChar;
                
                // Save
                if (lastChar == 's') {
                    
                    // Save file data
                    fsFileWrite(index, textBuffer, fileSize);
                    
                    ConsoleSetCursor(3, 15);
                    print(msgSaved, sizeof(msgSaved));
                    
                }
                
                // Quit
                if (lastChar == 'q') {
                    
                    ConsoleClearScreen();
                    ConsoleSetCursor(0, 0);
                    
                    lastChar = ConsoleGetLastChar();
                    
                    ConsoleSetBlinkRate(CURSOR_BLINK_RATE);
                    
                    return;
                }
                
                // Escape
                if (lastChar == 0x07) 
                    answere = 2;
                
                continue;
            }
            
            // Quit the application
            if (answere == 2) {
                
                ConsoleClearScreen();
            }
            
            ConsoleSetBlinkRate(CURSOR_BLINK_RATE);
        }
        
        // Cursor down
        if (lastChar == 0x04) {
            if (cursorLine < 3)
                cursorLine++;
            
        }
        
        // Cursor left
        if (lastChar == 0x05) {
            if (cursorPos > 0) {
                cursorPos--;
            } else {
                if (cursorLine > 0) {
                    cursorPos = 19;
                    cursorLine--;
                }
            }
        }
        
        // Cursor up
        if (lastChar == 0x03) {
            if (cursorLine > 0) 
                cursorLine--;
            
        }
        
        // Cursor right
        if (lastChar == 0x06) {
            if (cursorPos < 19) {
                cursorPos++;
            } else {
                if (cursorLine < 3) {
                    cursorPos = 0;
                    cursorLine++;
                }
            }
        }
        
        // Delete
        if (lastChar == 0x10) {
            
            textBuffer[cursorPos + (cursorLine * 20)] = ' ';
            
        }
        
        // Backspace
        if (lastChar == 0x01) {
            
            if (cursorPos > 0) {
                
                cursorPos--;
                
            } else {
                
                if (cursorLine > 0) {
                    
                    cursorLine--;
                    
                    cursorPos = 19;
                    
                }
                
            }
            
            ConsoleSetCursor(cursorLine, cursorPos);
            
            textBuffer[cursorPos + (cursorLine * 20)] = ' ';
            
        }
        
        //
        // Place character
        
        if ((lastChar > 0x19) ) {
            
            uint8_t pos = cursorPos + (cursorLine * 20);
            
            if (pos <= fileSize) 
                textBuffer[pos] = lastChar;
            
            if (cursorPos < 19) {
                
                cursorPos++;
                
            } else {
                
                if (cursorLine < 3) {
                    cursorPos = 0;
                    cursorLine++;
                }
                
            }
            
        }
        
        
        //
        // Draw the text buffer
        ConsoleSetCursor(0, 0);
        
        printk( textBuffer );
        
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
