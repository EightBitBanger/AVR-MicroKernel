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
    
    uint8_t msgPromptString[]    = "-";
    
    uint8_t asm_console_string[40];
    uint8_t asm_console_string_length = 0;
    
    uint32_t currentFileAddress = 0;
    
    uint8_t assemblyState = 0;
    uint32_t assemblyAddress = 0;
    
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
            
            if (asm_console_string_length == 0) {
                
                assemblyState = 0;
            }
            
            if (assemblyState == 0) {
                
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
                
                // Assembly address
                if (asm_console_string[0] == 'a') {
                    asm_console_string[0] = ' ';
                    
                    if ((is_number(&asm_console_string[2]) & is_number(&asm_console_string[3])) & 
                        (is_number(&asm_console_string[4]) & is_number(&asm_console_string[5]))) {
                        
                        uint8_t address[4];
                        
                        address[0] = asm_console_string[2];
                        address[1] = asm_console_string[3];
                        address[2] = asm_console_string[4];
                        address[3] = asm_console_string[5];
                        
                        asm_console_string[5] = ' ';
                        asm_console_string[4] = ' ';
                        asm_console_string[3] = ' ';
                        asm_console_string[2] = ' ';
                        
                        uint32_t integer = string_get_int_long(address);
                        
                        assemblyAddress = integer;
                        
                        currentFileAddress = integer;
                        
                        assemblyState = 1;
                        
                        // Drop an assembly prompt
                        uint8_t assemblyAddressString[4];
                        
                        assemblyAddressString[0] = ' ';
                        assemblyAddressString[1] = ' ';
                        assemblyAddressString[2] = ' ';
                        assemblyAddressString[3] = ' ';
                        
                        int_to_string(assemblyAddress, assemblyAddressString);
                        
                        uint8_t assemblyAddressStringCorrected[4];
                        
                        assemblyAddressStringCorrected[0] = assemblyAddressString[0];
                        assemblyAddressStringCorrected[1] = assemblyAddressString[1];
                        assemblyAddressStringCorrected[2] = assemblyAddressString[2];
                        assemblyAddressStringCorrected[3] = assemblyAddressString[3];
                        
                        for (uint8_t i=0; i < 4; i++) {
                            if (assemblyAddressStringCorrected[0] == ' ') {
                                assemblyAddressStringCorrected[0] = assemblyAddressStringCorrected[1];
                                assemblyAddressStringCorrected[1] = assemblyAddressStringCorrected[2];
                                assemblyAddressStringCorrected[2] = assemblyAddressStringCorrected[3];
                                assemblyAddressStringCorrected[3] = '0';
                            }
                        }
                        
                        for (uint8_t i=0; i < 4; i++) {
                            if (assemblyAddressStringCorrected[3] == ' ') {
                                assemblyAddressStringCorrected[3] = assemblyAddressStringCorrected[2];
                                assemblyAddressStringCorrected[2] = assemblyAddressStringCorrected[1];
                                assemblyAddressStringCorrected[1] = assemblyAddressStringCorrected[0];
                                assemblyAddressStringCorrected[0] = '0';
                            }
                        }
                        
                        print(assemblyAddressStringCorrected, 5);
                        
                        console_line = ConsoleGetCursorLine();
                        ConsoleSetCursor(console_line, 4);
                        printSpace(1);
                        
                        console_position = 5;
                        ConsoleSetCursor(console_line, console_position);
                        
                        asm_console_string_length = 0;
                        
                        continue;
                    }
                }
                
                // Dump
                if (asm_console_string[0] == 'd') {
                    asm_console_string[0] = ' ';
                    
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
                
                // Un-assemble
                if (asm_console_string[0] == 'u') {
                    asm_console_string[0] = ' ';
                    
                    if ((is_number(&asm_console_string[2]) & is_number(&asm_console_string[3])) & 
                        (is_number(&asm_console_string[4]) & is_number(&asm_console_string[5]))) {
                        
                        uint8_t address[4];
                        
                        address[0] = asm_console_string[2];
                        address[1] = asm_console_string[3];
                        address[2] = asm_console_string[4];
                        address[3] = asm_console_string[5];
                        
                        asm_console_string[5] = ' ';
                        asm_console_string[4] = ' ';
                        asm_console_string[3] = ' ';
                        asm_console_string[2] = ' ';
                        
                        currentFileAddress = string_get_int_long(address);
                        
                    }
                    
                    for (uint8_t i=0; i < 3; i++) {
                        
                        // Drop an assembly prompt
                        uint8_t assemblyAddressString[4];
                        
                        assemblyAddressString[0] = ' ';
                        assemblyAddressString[1] = ' ';
                        assemblyAddressString[2] = ' ';
                        assemblyAddressString[3] = ' ';
                        
                        int_to_string(currentFileAddress, assemblyAddressString);
                        
                        uint8_t assemblyAddressStringCorrected[4];
                        
                        assemblyAddressStringCorrected[0] = assemblyAddressString[0];
                        assemblyAddressStringCorrected[1] = assemblyAddressString[1];
                        assemblyAddressStringCorrected[2] = assemblyAddressString[2];
                        assemblyAddressStringCorrected[3] = assemblyAddressString[3];
                        
                        for (uint8_t i=0; i < 4; i++) {
                            if (assemblyAddressStringCorrected[0] == ' ') {
                                assemblyAddressStringCorrected[0] = assemblyAddressStringCorrected[1];
                                assemblyAddressStringCorrected[1] = assemblyAddressStringCorrected[2];
                                assemblyAddressStringCorrected[2] = assemblyAddressStringCorrected[3];
                                assemblyAddressStringCorrected[3] = '0';
                            }
                        }
                        
                        for (uint8_t i=0; i < 4; i++) {
                            if (assemblyAddressStringCorrected[3] == ' ') {
                                assemblyAddressStringCorrected[3] = assemblyAddressStringCorrected[2];
                                assemblyAddressStringCorrected[2] = assemblyAddressStringCorrected[1];
                                assemblyAddressStringCorrected[1] = assemblyAddressStringCorrected[0];
                                assemblyAddressStringCorrected[0] = '0';
                            }
                        }
                        
                        print(assemblyAddressStringCorrected, 5);
                        
                        console_line = ConsoleGetCursorLine();
                        
                        ConsoleSetCursor(console_line, 4);
                        printSpace(1);
                        
                        console_position = 5;
                        ConsoleSetCursor(console_line, console_position);
                        
                        asm_console_string_length = 0;
                        
                        //
                        // Interpret machine code into op-codes
                        //
                        uint8_t opCodeWasFound = 0;
                        
                        if (fileBuffer[currentFileAddress] == 0x20) {printc("nop", 4);  opCodeWasFound = 1;}
                        if (fileBuffer[currentFileAddress] == 0x4A) {printc("mov", 4);  opCodeWasFound = 3;}
                        if (fileBuffer[currentFileAddress] == 0x10) {printc("inc", 4);  opCodeWasFound = 2;}
                        if (fileBuffer[currentFileAddress] == 0x11) {printc("dec", 4);  opCodeWasFound = 2;}
                        if (fileBuffer[currentFileAddress] == 0x13) {printc("add", 4);  opCodeWasFound = 1;}
                        if (fileBuffer[currentFileAddress] == 0x14) {printc("sub", 4);  opCodeWasFound = 1;}
                        if (fileBuffer[currentFileAddress] == 0x5A) {printc("pop", 4);  opCodeWasFound = 2;}
                        if (fileBuffer[currentFileAddress] == 0x5C) {printc("push", 5); opCodeWasFound = 2;}
                        if (fileBuffer[currentFileAddress] == 0x5F) {printc("jmp", 4);  opCodeWasFound = 5;}
                        
                        // Unknown op-code
                        if (opCodeWasFound == 0) {
                            
                            printc("???", 4);
                            
                            currentFileAddress++;
                            
                        } else {
                            
                            currentFileAddress += opCodeWasFound;
                            
                            // Print the opcode arguments
                            if (opCodeWasFound == 2) {
                                
                                uint8_t argA = fileBuffer[currentFileAddress - 1];
                                
                                uint8_t argumentChar[2] = {'a', 'x'};
                                
                                argumentChar[0] += argA;
                                
                                printSpace(1);
                                print(argumentChar, 3);
                                
                            }
                            
                            if (opCodeWasFound == 3) {
                                
                                uint8_t argA = fileBuffer[currentFileAddress - 2];
                                uint8_t argB = fileBuffer[currentFileAddress - 1];
                                
                                uint8_t argumentCharA[2] = {'a', 'x'};
                                uint8_t argumentCharB[2] = {'a', 'x'};
                                
                                argumentCharA[0] += argA;
                                argumentCharB[0] += argB;
                                
                                printSpace(1);
                                print(argumentCharA, 3);
                                printc(",", 2);
                                print(argumentCharB, 3);
                                
                            }
                            
                        }
                        
                        printLn();
                        
                    }
                    
                }
                
                
                // Print the prompt line
                
                print(msgPromptString, sizeof(msgPromptString));
                
                console_position = 1;
                console_line = ConsoleGetCursorLine();
                ConsoleSetCursor(console_line, console_position);
                
                asm_console_string_length = 0;
                
            } else {
                
                // Assembly prompt
                
                //
                // Op-code to machine code translation for disassembly
                //
                
                uint8_t opCode[4] = {asm_console_string[0], asm_console_string[1], asm_console_string[2], asm_console_string[3]};
                uint8_t argCount = 0;
                
                if ((opCode[0] == 'n') & (opCode[1] == 'o') & (opCode[2] == 'p')) {fileBuffer[assemblyAddress] = 0x20; assemblyAddress += 1; argCount = 0;}
                if ((opCode[0] == 'm') & (opCode[1] == 'o') & (opCode[2] == 'v')) {fileBuffer[assemblyAddress] = 0x4A; assemblyAddress += 3; argCount = 2;}
                if ((opCode[0] == 'i') & (opCode[1] == 'n') & (opCode[2] == 'c')) {fileBuffer[assemblyAddress] = 0x10; assemblyAddress += 2; argCount = 1;}
                if ((opCode[0] == 'd') & (opCode[1] == 'e') & (opCode[2] == 'c')) {fileBuffer[assemblyAddress] = 0x11; assemblyAddress += 2; argCount = 1;}
                if ((opCode[0] == 'a') & (opCode[1] == 'd') & (opCode[2] == 'd')) {fileBuffer[assemblyAddress] = 0x13; assemblyAddress += 1; argCount = 0;}
                if ((opCode[0] == 's') & (opCode[1] == 'u') & (opCode[2] == 'b')) {fileBuffer[assemblyAddress] = 0x14; assemblyAddress += 1; argCount = 0;}
                if ((opCode[0] == 'p') & (opCode[1] == 'o') & (opCode[2] == 'p')) {fileBuffer[assemblyAddress] = 0x5A; assemblyAddress += 2; argCount = 1;}
                if ((opCode[0] == 'j') & (opCode[1] == 'm') & (opCode[2] == 'p')) {fileBuffer[assemblyAddress] = 0x5F; assemblyAddress += 5; argCount = 4;}
                
                if ((opCode[0] == 'p') & (opCode[1] == 'u') & (opCode[2] == 's') & (opCode[3] == 'h')) {fileBuffer[assemblyAddress] = 0x5C; assemblyAddress += 2; argCount = 1;}
                
                // Arguments
                uint8_t argA[4] = {0, 0, 0, 0};
                uint8_t argB[4] = {0, 0, 0, 0};
                
                // 4 digit opcodes
                if ((is_letter(&asm_console_string[5]) == 1) & (is_letter(&asm_console_string[6]) == 1)) {
                    argA[0] = asm_console_string[5];
                    argA[1] = asm_console_string[6];
                }
                
                // 3 digit opcodes
                if ((is_letter(&asm_console_string[4]) == 1) & (is_letter(&asm_console_string[5]) == 1)) {
                    argA[0] = asm_console_string[4];
                    argA[1] = asm_console_string[5];
                    argA[2] = asm_console_string[6];
                    argA[3] = asm_console_string[7];
                }
                
                if ((is_letter(&asm_console_string[7]) == 1) & (is_letter(&asm_console_string[8]) == 1)) {
                    argB[0] = asm_console_string[7];
                    argB[1] = asm_console_string[8];
                }
                
                
                //
                // Disassemble the opcode arguments
                //
                
                if (argCount == 1) {
                    uint8_t regIndex = 0;
                    
                    if ((argA[0] == 'a') & (argA[1] == 'x')) regIndex = 0;
                    if ((argA[0] == 'b') & (argA[1] == 'x')) regIndex = 1;
                    if ((argA[0] == 'c') & (argA[1] == 'x')) regIndex = 2;
                    if ((argA[0] == 'd') & (argA[1] == 'x')) regIndex = 3;
                    
                    fileBuffer[assemblyAddress - 1] = regIndex;
                    
                }
                
                if (argCount == 2) {
                    uint8_t regIndexA = 0;
                    uint8_t regIndexB = 0;
                    
                    if ((argA[0] == 'a') & (argA[1] == 'x')) regIndexA = 0;
                    if ((argA[0] == 'b') & (argA[1] == 'x')) regIndexA = 1;
                    if ((argA[0] == 'c') & (argA[1] == 'x')) regIndexA = 2;
                    if ((argA[0] == 'd') & (argA[1] == 'x')) regIndexA = 3;
                    
                    if ((argB[0] == 'a') & (argB[1] == 'x')) regIndexB = 0;
                    if ((argB[0] == 'b') & (argB[1] == 'x')) regIndexB = 1;
                    if ((argB[0] == 'c') & (argB[1] == 'x')) regIndexB = 2;
                    if ((argB[0] == 'd') & (argB[1] == 'x')) regIndexB = 3;
                    
                    fileBuffer[assemblyAddress - 2] = regIndexA;
                    fileBuffer[assemblyAddress - 1] = regIndexB;
                    
                }
                
                if (argCount == 4) {
                    union Pointer ptr;
                    
                    ptr.address = string_get_int_long(argA);
                    
                    fileBuffer[assemblyAddress - 4] = ptr.byte_t[0];
                    fileBuffer[assemblyAddress - 3] = ptr.byte_t[1];
                    fileBuffer[assemblyAddress - 2] = ptr.byte_t[2];
                    fileBuffer[assemblyAddress - 1] = ptr.byte_t[3];
                    
                }
                
                
                // Clear the old op codes
                for (uint8_t i=0; i < 4; i++) {
                    opCode[i] = ' ';
                    asm_console_string[i] = ' ';
                }
                
                uint8_t assemblyAddressString[4];
                
                assemblyAddressString[0] = ' ';
                assemblyAddressString[1] = ' ';
                assemblyAddressString[2] = ' ';
                assemblyAddressString[3] = ' ';
                
                int_to_string(assemblyAddress, assemblyAddressString);
                
                uint8_t assemblyAddressStringCorrected[4];
                
                assemblyAddressStringCorrected[0] = assemblyAddressString[0];
                assemblyAddressStringCorrected[1] = assemblyAddressString[1];
                assemblyAddressStringCorrected[2] = assemblyAddressString[2];
                assemblyAddressStringCorrected[3] = assemblyAddressString[3];
                
                for (uint8_t i=0; i < 4; i++) {
                    if (assemblyAddressStringCorrected[0] == ' ') {
                        assemblyAddressStringCorrected[0] = assemblyAddressStringCorrected[1];
                        assemblyAddressStringCorrected[1] = assemblyAddressStringCorrected[2];
                        assemblyAddressStringCorrected[2] = assemblyAddressStringCorrected[3];
                        assemblyAddressStringCorrected[3] = '0';
                    }
                }
                
                for (uint8_t i=0; i < 4; i++) {
                    if (assemblyAddressStringCorrected[3] == ' ') {
                        assemblyAddressStringCorrected[3] = assemblyAddressStringCorrected[2];
                        assemblyAddressStringCorrected[2] = assemblyAddressStringCorrected[1];
                        assemblyAddressStringCorrected[1] = assemblyAddressStringCorrected[0];
                        assemblyAddressStringCorrected[0] = '0';
                    }
                }
                
                print(assemblyAddressStringCorrected, 5);
                
                console_line = ConsoleGetCursorLine();
                
                ConsoleSetCursor(console_line, 4);
                printSpace(1);
                
                console_position = 5;
                ConsoleSetCursor(console_line, console_position);
                
                asm_console_string_length = 0;
                
            }
            
            continue;
        }
        
        // Backspace
        if (lastChar == 0x01) {
            
            if (asm_console_string_length > 0) {
                
                if (assemblyState == 0) {
                    ConsoleSetCursorPosition(asm_console_string_length);
                } else {
                    ConsoleSetCursorPosition(asm_console_string_length + 4);
                }
                
                printChar(' ');
                asm_console_string[asm_console_string_length] = ' ';
                
                ConsoleSetCursorPosition(asm_console_string_length);
                
                asm_console_string_length--;
                
            }
            
            if (assemblyState == 0) {
                ConsoleSetCursorPosition(1);
            } else {
                ConsoleSetCursorPosition(5);
            }
            
            print(asm_console_string, asm_console_string_length + 1);
            
            continue;
        }
        
        // Escape exit
        //if (lastChar == 0x07) {
        //    break;
        //}
        
        // Place a character
        if (lastChar > 0x19) {
            
            asm_console_string[asm_console_string_length] = lastChar;
            asm_console_string_length++;
            
            if (assemblyState == 0) {
                ConsoleSetCursorPosition(1);
            } else {
                ConsoleSetCursorPosition(5);
            }
            
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
