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
    uint8_t textLineA[35];
    uint8_t textLineB[35];
    uint8_t textLineC[35];
    uint8_t textLineD[35];
    
    for (uint8_t i=0; i < 20; i++) {
        textLineA[i] = ' ';
        textLineB[i] = ' ';
        textLineC[i] = ' ';
        textLineD[i] = ' ';
    }
    
    // Null terminate the string
    uint8_t textBuffer[fileSize];
    for (uint16_t i=0; i < fileSize; i++) 
        textBuffer[i] = ' ';
    
    // Load the file
    fsFileRead(index, textBuffer, fileSize);
    
    // Split file text into lines
    
    uint8_t line     = 0;
    uint8_t position = 0;
    
    for (uint8_t i=0; i < fileSize; i++) {
        
        if (line > 3) 
            break;
        
        if (line == 0) textLineA[position] = textBuffer[i];
        if (line == 1) textLineB[position] = textBuffer[i];
        if (line == 2) textLineC[position] = textBuffer[i];
        if (line == 3) textLineD[position] = textBuffer[i];
        
        if (textBuffer[i] == '\n') {
            line++;
            position = 0;
        } else {
            
            position++;
            if (position > 20) {
                line++;
                position = 0;
            }
        }
    }
    
    
    ConsoleClearScreen();
    
    ConsoleSetCursor(0, 0);
    
    uint8_t isActive = 1;
    while (isActive) {
        
        uint8_t currentChar = ConsoleGetRawChar();
        
        if (lastChar == currentChar) 
            continue;
        
        lastChar = currentChar;
        
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
        
        // Check EOL
        uint8_t lineEOL = 0;
        if (cursorLine == 0) {
            for (uint8_t i=20; i > 0; i--) 
                if (textLineA[i] == '\n') {lineEOL = i; break;}
            if (cursorPos > lineEOL) cursorPos = lineEOL;
        }
        if (cursorLine == 1) {
            for (uint8_t i=20; i > 0; i--) 
                if (textLineB[i] == '\n') {lineEOL = i; break;}
            if (cursorPos > lineEOL) cursorPos = lineEOL;
        }
        if (cursorLine == 2) {
            for (uint8_t i=20; i > 0; i--) 
                if (textLineC[i] == '\n') {lineEOL = i; break;}
            if (cursorPos > lineEOL) cursorPos = lineEOL;
        }
        if (cursorLine == 3) {
            for (uint8_t i=20; i > 0; i--) 
                if (textLineD[i] == '\n') {lineEOL = i; break;}
            if (cursorPos > lineEOL) cursorPos = lineEOL;
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
            
            // Shift down the EOL character
            if (cursorLine == 0) {
                
                uint8_t posEOL = 0;
                
                for (uint8_t i=0; i < 20; i++) {
                    posEOL = i;
                    if (textLineA[i] == '\n') 
                        break;
                }
                
                for (uint8_t i=posEOL; i >= 0; i--) {
                    textLineA[cursorPos + i + 1] = textLineA[cursorPos + i];
                    
                    //if (textLineA[cursorPos + i] == '\n') {
                    //    break;
                    //}
                    
                    if (i == 0) 
                        break;
                }
                
            }
            
            if ((textLineA[20] != '\n') & (cursorLine == 0)) textLineA[cursorPos] = lastChar;
            if ((textLineB[20] != '\n') & (cursorLine == 1)) textLineB[cursorPos] = lastChar;
            if ((textLineC[20] != '\n') & (cursorLine == 2)) textLineC[cursorPos] = lastChar;
            if ((textLineD[20] != '\n') & (cursorLine == 3)) textLineD[cursorPos] = lastChar;
            
            cursorPos++;
            
            if (cursorPos > 20) 
                cursorPos = 20;
            
        }
        
        
        // Escape exit
        if (lastChar == 0x07) {
            
            // Save/Quit/Cancel menu option
            
            ConsoleClearScreen();
            
            ConsoleSetCursor(0, 0);
            
            print(msgSaveQuit, sizeof(msgSaveQuit));
            
            ConsoleSetCursor(2, 0);
            
            print(filename, filename_length + 1);
            
            ConsoleSetCursor(3, 0);
            printInt( fileSize );
            
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
                    
                    uint8_t saveBuffer[fileSize];
                    
                    for (uint16_t i=0; i < fileSize; i++) 
                        saveBuffer[i] = ' ';
                    uint8_t saveIndex = 0;
                    
                    for (uint8_t i=0; i < 20; i++) {
                        saveBuffer[saveIndex] = textLineA[i];
                        saveIndex++;
                        if (textLineA[i] == '\n') break;
                    }
                    for (uint8_t i=0; i < 20; i++) {
                        saveBuffer[saveIndex] = textLineB[i];
                        saveIndex++;
                        if (textLineB[i] == '\n') break;
                    }
                    for (uint8_t i=0; i < 20; i++) {
                        saveBuffer[saveIndex] = textLineC[i];
                        saveIndex++;
                        if (textLineC[i] == '\n') break;
                    }
                    for (uint8_t i=0; i < 20; i++) {
                        saveBuffer[saveIndex] = textLineD[i];
                        saveIndex++;
                        if (textLineD[i] == '\n') break;
                    }
                    
                    // Save file data
                    fsFileWrite(index, saveBuffer, fileSize);
                    
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
        
        
        //
        // Draw the text buffer
        ConsoleSetCursor(0, 0);
        
        //printk( textBuffer );
        
        print(textLineA, 21);
        printLn();
        print(textLineB, 21);
        printLn();
        print(textLineC, 21);
        printLn();
        print(textLineD, 21);
        
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
