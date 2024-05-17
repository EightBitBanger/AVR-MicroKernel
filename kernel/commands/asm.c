#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/commands/list.h>

void functionAsm(uint8_t* param, uint8_t param_length) {
    
    if (fsFileExists(param, param_length) == 0) 
        return;
    
    uint32_t fileSize = fsGetFileSize(param, param_length);
    
    uint8_t lastChar =  ConsoleGetRawChar();
    uint8_t console_position = ConsoleGetCursorPosition();
    uint8_t console_line     = ConsoleGetCursorLine();
    
    uint8_t msgPromptString[] = "-";
    
    uint8_t asm_console_string[40];
    uint8_t asm_console_string_length = 0;
    
    uint32_t currentFileAddress = 0;
    
    uint8_t assemblyState = 0;
    uint8_t assemblyAddress = 0;
    
    uint8_t fileBuffer[fileSize];
    for (uint16_t i=0; i < fileSize; i++) 
        fileBuffer[i] = ' ';
    
    print(msgPromptString, sizeof(msgPromptString));
    
    console_position = 1;
    ConsoleSetCursor(console_line, console_position);
    
    // Open the file
    uint8_t index = fsFileOpen(param, param_length);
    
    // Load the file into memory
    fsFileRead(index, fileBuffer, fileSize);
    
    while(1) {
        
        uint8_t currentChar = ConsoleGetRawChar();
        
        if (lastChar == currentChar) 
            continue;
        
        lastChar = currentChar;
        
        // Return
        if (lastChar == 0x02) {
            
            printLn();
            
            // Quit
            if (asm_console_string[0] == 'q') 
                break;
            
            // Write data to file
            if (asm_console_string[0] == 'w') {
                
                fsFileWrite(index, fileBuffer, fileSize);
                
                uint8_t msgFileWritten[] = "File written";
                
                print(msgFileWritten, sizeof(msgFileWritten));
                printLn();
                
            }
            
            // Set assembling address
            if (asm_console_string[0] == 'a') {
                asm_console_string[0] = ' ';
                
                // Set dump range
                if (is_number(&asm_console_string[2]) & is_number(&asm_console_string[3])) {
                    
                    uint8_t address[3];
                    
                    address[0] = asm_console_string[2];
                    address[1] = asm_console_string[3];
                    address[2] = '0';
                    
                    uint8_t integer = string_get_int(address);
                    
                    assemblyAddress    = integer;
                    
                    currentFileAddress = integer;
                    
                }
            }
            
            // Dump
            if (asm_console_string[0] == 'd') {
                asm_console_string[0] = ' ';
                
                // Set dump range
                if (is_number(&asm_console_string[2]) & is_number(&asm_console_string[3])) {
                    
                    uint8_t address[3];
                    
                    address[0] = asm_console_string[2];
                    address[1] = asm_console_string[3];
                    address[2] = '0';
                    
                    uint8_t integer = string_get_int(address);
                    
                    currentFileAddress = integer;
                    
                }
                
                uint8_t hexStringA[] = "00000000-00000000";
                uint8_t hexStringB[] = "00000000-00000000";
                uint8_t hexStringC[] = "00000000-00000000";
                
                for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + currentFileAddress], &hexStringA[i * 2] );
                for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 4  + currentFileAddress], &hexStringA[(i * 2) + 9] );
                for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 8  + currentFileAddress], &hexStringB[(i * 2)] );
                for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 12 + currentFileAddress], &hexStringB[(i * 2) + 9] );
                for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 16 + currentFileAddress], &hexStringC[(i * 2)] );
                for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 20 + currentFileAddress], &hexStringC[(i * 2) + 9] );
                
                currentFileAddress += 24;
                
                print(hexStringA, sizeof(hexStringA));
                printLn();
                print(hexStringB, sizeof(hexStringB));
                printLn();
                print(hexStringC, sizeof(hexStringC));
                printLn();
                
            }
            
            // Disassemble
            if (asm_console_string[0] == 'u') {
                asm_console_string[0] = ' ';
                
                uint8_t hexStringA[] = "mov ax, bx";
                uint8_t hexStringB[] = "int 10";
                uint8_t hexStringC[] = "int 20";
                
                print(hexStringA, sizeof(hexStringA));
                printLn();
                print(hexStringB, sizeof(hexStringB));
                printLn();
                print(hexStringC, sizeof(hexStringC));
                printLn();
                
            }
            
            // Print the prompt line
            print(msgPromptString, sizeof(msgPromptString));
            
            console_position = 1;
            console_line = ConsoleGetCursorLine();
            ConsoleSetCursor(console_line, console_position);
            
            asm_console_string_length = 0;
            
            continue;
        }
        
        // Backspace
        if (lastChar == 0x01) {
            
            if (asm_console_string_length > 0) {
                
                ConsoleSetCursorPosition(asm_console_string_length);
                
                printChar(' ');
                asm_console_string[asm_console_string_length] = ' ';
                
                ConsoleSetCursorPosition(asm_console_string_length);
                
                asm_console_string_length--;
                
            }
            
            ConsoleSetCursorPosition(1);
            print(asm_console_string, asm_console_string_length + 1);
            
            continue;
        }
        
        // Escape exit
        if (lastChar == 0x07) {
            
            break;
        }
        
        // Place a charactor
        if (lastChar > 0x19) {
            
            asm_console_string[asm_console_string_length] = lastChar;
            asm_console_string_length++;
            
            ConsoleSetCursorPosition(1);
            print(asm_console_string, asm_console_string_length + 1);
            
        }
        
        continue;
    }
    
    return;
}



void registerCommandAssembly(void) {
    
    uint8_t asmCommandName[] = "asm";
    
    ConsoleRegisterCommand(asmCommandName, sizeof(asmCommandName), functionAsm);
    
    return;
}
