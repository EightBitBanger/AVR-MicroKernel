#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/list/list.h>

void functionAsm(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgPromptString[] = "-";
    
    uint8_t asm_console_string[40];
    
    uint8_t asm_console_string_length;
    
    uint8_t assemblyState;
    uint32_t assemblyAddress;
    
    if (fsFileExists(param, param_length) == 0) 
        return;
    
    uint32_t fileSize = fsGetFileSize(param, param_length);
    
    uint8_t lastChar         = ConsoleGetRawChar();
    uint8_t console_position = ConsoleGetCursorPosition();
    uint8_t console_line     = ConsoleGetCursorLine();
    
    for (uint8_t i; i < 40; i++) 
        asm_console_string[i] = ' ';
    
    asm_console_string_length = 0;
    
    assemblyState   = 0;
    assemblyAddress = 0;
    
    // File data buffer
    uint8_t fileBuffer[fileSize];
    
    for (uint16_t i=0; i < fileSize; i++) 
        fileBuffer[i] = ' ';
    
    print(msgPromptString, sizeof(msgPromptString));
    
    console_position = 1;
    ConsoleSetCursor(console_line, console_position);
    
    // Open the file
    uint8_t index = fsFileOpen(param, param_length);
    
    // Load the file into memory
    fsFileReadBin(index, fileBuffer, fileSize);
    
    
    
    void getInputAddress(void) {
        
        if ((is_number(&asm_console_string[2]) == 1) & 
            (is_number(&asm_console_string[3]) == 1) & 
            (is_number(&asm_console_string[4]) == 1) & 
            (is_number(&asm_console_string[5]) == 1)) {
            
            uint8_t address[4];
            
            address[0] = asm_console_string[2];
            address[1] = asm_console_string[3];
            address[2] = asm_console_string[4];
            address[3] = asm_console_string[5];
            
            assemblyAddress = string_get_int_long(address);
        }
        
        if ((is_number(&asm_console_string[2]) == 1) & 
            (is_number(&asm_console_string[3]) == 1) & 
            (is_number(&asm_console_string[4]) == 1) & 
            (is_number(&asm_console_string[5]) == 0)) {
            
            uint8_t address[4];
            
            address[0] = '0';
            address[1] = asm_console_string[2];
            address[2] = asm_console_string[3];
            address[3] = asm_console_string[4];
            
            assemblyAddress = string_get_int_long(address);
        }
        
        if ((is_number(&asm_console_string[2]) == 1) & 
            (is_number(&asm_console_string[3]) == 1) & 
            (is_number(&asm_console_string[4]) == 0) & 
            (is_number(&asm_console_string[5]) == 0)) {
            
            uint8_t address[4];
            
            address[0] = '0';
            address[1] = '0';
            address[2] = asm_console_string[2];
            address[3] = asm_console_string[3];
            
            assemblyAddress = string_get_int_long(address);
        }
        
        if ((is_number(&asm_console_string[2]) == 1) & 
            (is_number(&asm_console_string[3]) == 0) & 
            (is_number(&asm_console_string[4]) == 0) & 
            (is_number(&asm_console_string[5]) == 0)) {
            
            uint8_t address[4];
            
            address[0] = '0';
            address[1] = '0';
            address[2] = '0';
            address[3] = asm_console_string[2];
            
            assemblyAddress = string_get_int_long(address);
        }
        
        asm_console_string[5] = ' ';
        asm_console_string[4] = ' ';
        asm_console_string[3] = ' ';
        asm_console_string[2] = ' ';
        
        return;
    }
    
    
    void printAddressLine(void) {
        
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
        printSpace(1);
        
        console_line = ConsoleGetCursorLine();
        
        console_position = 5;
        ConsoleSetCursor(console_line, console_position);
        
        return;
    }
    
    
    //
    // Application entry point
    //
    
    while(1) {
        
        uint8_t currentChar = ConsoleGetRawChar();
        
        if (lastChar == currentChar) 
            continue;
        
        lastChar = currentChar;
        
        // Return
        if (lastChar == 0x02) {
            
            printLn();
            
            if (asm_console_string_length == 0) 
                assemblyState = 0;
            
            if (assemblyState == 0) {
                
                //
                // Quit
                //
                
                if (asm_console_string[0] == 'q') 
                    break;
                
                
                //
                // Run the program in memory
                //
                
                if (asm_console_string[0] == 'r') {
                    
                    uint8_t statusCode = EmulateX4( fileBuffer, fileSize );
                    
                    if (statusCode != 1) {
                        
                        uint8_t msgError[] = "error: ";
                        
                        print(msgError, sizeof(msgError));
                        printInt( statusCode );
                        printLn();
                    }
                    
                }
                
                
                //
                // Write data to file
                //
                
                if (asm_console_string[0] == 'w') {
                    
                    fsFileWrite(index, fileBuffer, fileSize);
                    printInt( fileSize );
                    
                    if (fileSize == 1) {
                        uint8_t msgByte[] = "byte ";
                        print(msgByte, sizeof(msgByte));
                    } else {
                        uint8_t msgBytes[] = "bytes ";
                        print(msgBytes, sizeof(msgBytes));
                    }
                    
                    uint8_t msgWritten[] = "written";
                    print(msgWritten, sizeof(msgWritten));
                    
                    printLn();
                    
                }
                
                
                //
                // Assemble
                //
                
                if (asm_console_string[0] == 'a') {
                    asm_console_string[0] = ' ';
                    
                    getInputAddress();
                    
                    assemblyState = 1;
                    
                    printAddressLine();
                    
                    asm_console_string_length = 0;
                    
                    continue;
                }
                
                
                //
                // Disassemble
                //
                
                if (asm_console_string[0] == 'u') {
                    asm_console_string[0] = ' ';
                    
                    getInputAddress();
                    
                    // Print the lines
                    for (uint8_t i=0; i < 3; i++) {
                        
                        printAddressLine();
                        
                        asm_console_string_length = 0;
                        
                        // Check end of file
                        if (assemblyAddress >= fileSize) {
                            
                            uint8_t msgEndOfFile[] = "[EOF]";
                            print(msgEndOfFile, sizeof(msgEndOfFile));
                            
                            printLn();
                            
                            break;
                        }
                        
                        //
                        // Interpret machine code into op-codes
                        //
                        
                        uint8_t opCodeWasFound = 0;
                        uint8_t specialOpcodeArgs = 0;
                        
                        if (fileBuffer[assemblyAddress] == 0x90) {printc("nop", 4);  opCodeWasFound = 1;}
                        if (fileBuffer[assemblyAddress] == 0xFD) {printc("inc", 4);  opCodeWasFound = 2;}
                        if (fileBuffer[assemblyAddress] == 0xFC) {printc("dec", 4);  opCodeWasFound = 2;}
                        
                        if (fileBuffer[assemblyAddress] == 0x88) {printc("mov", 4);  opCodeWasFound = 3;}
                        if (fileBuffer[assemblyAddress] == 0x89) {printc("mov", 4);  opCodeWasFound = 3; specialOpcodeArgs = 2;}
                        
                        if (fileBuffer[assemblyAddress] == 0x00) {printc("add", 4);  opCodeWasFound = 1;}
                        if (fileBuffer[assemblyAddress] == 0x80) {printc("sub", 4);  opCodeWasFound = 1;}
                        if (fileBuffer[assemblyAddress] == 0xF6) {printc("mul", 4);  opCodeWasFound = 1;}
                        if (fileBuffer[assemblyAddress] == 0xF4) {printc("div", 4);  opCodeWasFound = 1;}
                        
                        if (fileBuffer[assemblyAddress] == 0x0F) {printc("pop", 4);  opCodeWasFound = 2;}
                        if (fileBuffer[assemblyAddress] == 0xF0) {printc("push", 5); opCodeWasFound = 2;}
                        
                        if (fileBuffer[assemblyAddress] == 0xFE) {printc("jmp", 4);  opCodeWasFound = 5;}
                        if (fileBuffer[assemblyAddress] == 0x9A) {printc("call", 5);  opCodeWasFound = 5;}
                        
                        if (fileBuffer[assemblyAddress] == 0xCB) {printc("ret", 4);  opCodeWasFound = 1;}
                        
                        if ((fileBuffer[assemblyAddress] == 0xCC) | 
                            (fileBuffer[assemblyAddress] == 0xCD)) {printc("int", 4);  opCodeWasFound = 2; specialOpcodeArgs = 1;}
                        
                        if (fileBuffer[assemblyAddress] == 0xFA) {printc("cli", 4);  opCodeWasFound = 1;}
                        if (fileBuffer[assemblyAddress] == 0xFB) {printc("sti", 4);  opCodeWasFound = 1;}
                        
                        // Unknown op-code
                        if (opCodeWasFound == 0) {
                            
                            printc("???", 4);
                            
                            assemblyAddress++;
                            
                        } else {
                            
                            assemblyAddress += opCodeWasFound;
                            
                            //
                            // Print the opcode arguments
                            
                            
                            if (opCodeWasFound == 2) {
                                
                                uint8_t argA = fileBuffer[assemblyAddress - 1];
                                
                                uint8_t argumentChar[2] = {'a', 'x'};
                                
                                
                                printSpace(1);
                                
                                if (specialOpcodeArgs == 1) {
                                    
                                    int_to_hex_string(argA, argumentChar);
                                    
                                    print(argumentChar, 3);
                                    printChar('h');
                                    
                                } else {
                                    
                                    argumentChar[0] += argA;
                                    
                                    print(argumentChar, 3);
                                    
                                }
                                
                            }
                            
                            
                            
                            if (opCodeWasFound == 3) {
                                
                                uint8_t argA = fileBuffer[assemblyAddress - 2];
                                uint8_t argB = fileBuffer[assemblyAddress - 1];
                                
                                uint8_t argumentCharA[2] = {'a', 'x'};
                                uint8_t argumentCharB[2] = {'a', 'x'};
                                
                                argumentCharA[0] += argA;
                                
                                printSpace(1);
                                print(argumentCharA, 3);
                                printc(",", 2);
                                
                                // Move byte / register
                                if (specialOpcodeArgs == 2) {
                                    
                                    int_to_hex_string(argB, argumentCharB);
                                    
                                    print(argumentCharB, 3);
                                    printChar('h');
                                    
                                }
                                
                                // Move register to register
                                if (specialOpcodeArgs == 0) {
                                    
                                    argumentCharB[0] += argB;
                                    print(argumentCharB, 3);
                                }
                                
                            }
                            
                            if (opCodeWasFound == 5) {
                                
                                uint8_t argA[4];
                                argA[3] = fileBuffer[assemblyAddress - 4];
                                argA[2] = fileBuffer[assemblyAddress - 3];
                                argA[1] = fileBuffer[assemblyAddress - 2];
                                argA[0] = fileBuffer[assemblyAddress - 1];
                                
                                union Pointer ptr;
                                ptr.byte_t[0] = argA[0];
                                ptr.byte_t[1] = argA[1];
                                ptr.byte_t[2] = argA[2];
                                ptr.byte_t[3] = argA[3];
                                
                                uint8_t intString[10];
                                
                                for (uint8_t i=0; i < 10; i++) 
                                    intString[i] = ' ';
                                
                                int_to_string(ptr.address, intString);
                                
                                for (uint8_t i=0; i < 4; i++) {
                                    if (intString[0] == ' ') {
                                        intString[0] = intString[1];
                                        intString[1] = intString[2];
                                        intString[2] = intString[3];
                                        intString[3] = '0';
                                    }
                                }
                                
                                for (uint8_t i=0; i < 4; i++) {
                                    if (intString[3] == ' ') {
                                        intString[3] = intString[2];
                                        intString[2] = intString[1];
                                        intString[1] = intString[0];
                                        intString[0] = '0';
                                    }
                                }
                                
                                printSpace(1);
                                print(intString, 5);
                                
                            }
                            
                        }
                        
                        printLn();
                        
                    }
                    
                }
                
                
                //
                // Dump
                //
                
                if (asm_console_string[0] == 'd') {
                    asm_console_string[0] = ' ';
                    
                    getInputAddress();
                    
                    uint8_t hexStringA[17];
                    uint8_t hexStringB[17];
                    uint8_t hexStringC[17];
                    
                    hexStringA[8] = '-';
                    hexStringB[8] = '-';
                    hexStringC[8] = '-';
                    
                    for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + assemblyAddress], &hexStringA[i * 2] );
                    for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 4  + assemblyAddress], &hexStringA[(i * 2) + 9] );
                    for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 8  + assemblyAddress], &hexStringB[(i * 2)] );
                    for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 12 + assemblyAddress], &hexStringB[(i * 2) + 9] );
                    for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 16 + assemblyAddress], &hexStringC[(i * 2)] );
                    for (uint8_t i=0; i < 4; i++) int_to_hex_string( fileBuffer[i + 20 + assemblyAddress], &hexStringC[(i * 2) + 9] );
                    
                    assemblyAddress += 24;
                    
                    print(hexStringA, sizeof(hexStringA)+1);
                    printLn();
                    print(hexStringB, sizeof(hexStringB)+1);
                    printLn();
                    print(hexStringC, sizeof(hexStringC)+1);
                    printLn();
                    
                }
                
                
                //
                // Print the prompt line
                
                print(msgPromptString, sizeof(msgPromptString));
                
                console_position = 1;
                console_line = ConsoleGetCursorLine();
                ConsoleSetCursor(console_line, console_position);
                
                // Clear the old op codes
                for (uint8_t i=0; i < 4; i++) {
                    asm_console_string[i] = ' ';
                }
                
                asm_console_string_length = 0;
                
            } else {
                
                // Assembly prompt
                
                //
                // Op-code to machine code translation for disassembly
                //
                
                uint8_t opCode[4] = {asm_console_string[0], asm_console_string[1], asm_console_string[2], asm_console_string[3]};
                uint8_t argCount = 0;
                
                if ((opCode[0] == 'd') & (opCode[1] == 'b')) {
                    
                    for (uint8_t i=0; i < asm_console_string_length; i++) {
                        fileBuffer[assemblyAddress] = asm_console_string[3 + i];
                        assemblyAddress++;
                    }
                    
                    printAddressLine();
                    
                    asm_console_string_length = 0;
                    
                    // Clear the old op codes
                    for (uint8_t i=0; i < 4; i++) {
                        opCode[i] = ' ';
                        asm_console_string[i] = ' ';
                    }
                    
                    continue;
                }
                
                if ((opCode[0] == 'n') & (opCode[1] == 'o') & (opCode[2] == 'p')) {fileBuffer[assemblyAddress] = 0x90; assemblyAddress += 1; argCount = 0;}
                if ((opCode[0] == 'm') & (opCode[1] == 'o') & (opCode[2] == 'v')) {fileBuffer[assemblyAddress] = 0x88; assemblyAddress += 3; argCount = 2;}
                if ((opCode[0] == 'i') & (opCode[1] == 'n') & (opCode[2] == 'c')) {fileBuffer[assemblyAddress] = 0xFD; assemblyAddress += 2; argCount = 1;}
                if ((opCode[0] == 'd') & (opCode[1] == 'e') & (opCode[2] == 'c')) {fileBuffer[assemblyAddress] = 0xFC; assemblyAddress += 2; argCount = 1;}
                
                if ((opCode[0] == 'a') & (opCode[1] == 'd') & (opCode[2] == 'd')) {fileBuffer[assemblyAddress] = 0x00; assemblyAddress += 1; argCount = 0;}
                if ((opCode[0] == 's') & (opCode[1] == 'u') & (opCode[2] == 'b')) {fileBuffer[assemblyAddress] = 0x80; assemblyAddress += 1; argCount = 0;}
                if ((opCode[0] == 'm') & (opCode[1] == 'u') & (opCode[2] == 'l')) {fileBuffer[assemblyAddress] = 0xF6; assemblyAddress += 1; argCount = 0;}
                if ((opCode[0] == 'd') & (opCode[1] == 'i') & (opCode[2] == 'v')) {fileBuffer[assemblyAddress] = 0xF4; assemblyAddress += 1; argCount = 0;}
                
                if ((opCode[0] == 'p') & (opCode[1] == 'o') & (opCode[2] == 'p')) {fileBuffer[assemblyAddress] = 0x0F; assemblyAddress += 2; argCount = 1;}
                if ((opCode[0] == 'p') & (opCode[1] == 'u') & (opCode[2] == 's') & (opCode[3] == 'h')) {fileBuffer[assemblyAddress] = 0xF0; assemblyAddress += 2; argCount = 1;}
                
                if ((opCode[0] == 'j') & (opCode[1] == 'm') & (opCode[2] == 'p')) {fileBuffer[assemblyAddress] = 0xFE; assemblyAddress += 5; argCount = 4;}
                if ((opCode[0] == 'c') & (opCode[1] == 'a') & (opCode[2] == 'l') & (opCode[3] == 'l')) {fileBuffer[assemblyAddress] = 0x9A; assemblyAddress += 5; argCount = 4;}
                
                if ((opCode[0] == 'r') & (opCode[1] == 'e') & (opCode[2] == 't')) {fileBuffer[assemblyAddress] = 0xCB; assemblyAddress += 1; argCount = 0;}
                
                if ((opCode[0] == 'i') & (opCode[1] == 'n') & (opCode[2] == 't')) {fileBuffer[assemblyAddress] = 0xCC; assemblyAddress += 2; argCount = 1;}
                if ((opCode[0] == 'c') & (opCode[1] == 'l') & (opCode[2] == 'i')) {fileBuffer[assemblyAddress] = 0xFA; assemblyAddress += 1; argCount = 0;}
                if ((opCode[0] == 's') & (opCode[1] == 't') & (opCode[2] == 'i')) {fileBuffer[assemblyAddress] = 0xFB; assemblyAddress += 1; argCount = 0;}
                
                // Arguments
                uint8_t argA[4] = {0, 0, 0, 0};
                uint8_t argB[4] = {0, 0, 0, 0};
                uint8_t argAIsHex = 0;
                uint8_t argBIsHex = 0;
                
                uint8_t argBIsAddr = 0;
                
                // Arguments after a 4 digit opcode
                if ((is_letter(&asm_console_string[5]) == 1) & (is_letter(&asm_console_string[6]) == 1)) {
                    argA[0] = asm_console_string[5];
                    argA[1] = asm_console_string[6];
                }
                
                // 4 digit opcode with numeric arguments
                if ((is_number(&asm_console_string[5]) == 1) & (is_number(&asm_console_string[6]) == 1) & 
                    (is_number(&asm_console_string[7]) == 1) & (is_number(&asm_console_string[8]) == 1)) {
                    argA[0] = asm_console_string[5];
                    argA[1] = asm_console_string[6];
                    argA[2] = asm_console_string[7];
                    argA[3] = asm_console_string[8];
                }
                
                // Arguments after a 3 digit opcode
                if ((is_letter(&asm_console_string[4]) == 1) & (is_letter(&asm_console_string[5]) == 1)) {
                    argA[0] = asm_console_string[4];
                    argA[1] = asm_console_string[5];
                }
                
                if ((is_letter(&asm_console_string[7]) == 1) & (is_letter(&asm_console_string[8]) == 1)) {
                    argB[0] = asm_console_string[7];
                    argB[1] = asm_console_string[8];
                }
                
                // Check hex
                if ((is_hex(&asm_console_string[4]) == 1) & (is_hex(&asm_console_string[5]) == 1)) {
                    argA[0] = asm_console_string[4];
                    argA[1] = asm_console_string[5];
                    argAIsHex = 1;
                }
                
                if ((is_hex(&asm_console_string[7]) == 1) & (is_hex(&asm_console_string[8]) == 1)) {
                    argB[0] = asm_console_string[7];
                    argB[1] = asm_console_string[8];
                    argBIsHex = 1;
                }
                
                // Check argument B is address
                if ((is_number(&asm_console_string[7]) == 1) & 
                    (is_number(&asm_console_string[8]) == 1) & 
                    (is_number(&asm_console_string[9]) == 1) & 
                    (is_number(&asm_console_string[10]) == 1)) {
                    
                    argB[0] = asm_console_string[7];
                    argB[1] = asm_console_string[8];
                    argB[2] = asm_console_string[9];
                    argB[3] = asm_console_string[10];
                    
                    argBIsAddr = 1;
                }
                
                // Arguments after a 3 digit opcode with a numeric argument
                if ((is_number(&asm_console_string[4]) == 1) & (is_number(&asm_console_string[5]) == 1) & 
                    (is_number(&asm_console_string[6]) == 1) & (is_number(&asm_console_string[7]) == 1)) {
                    argA[0] = asm_console_string[4];
                    argA[1] = asm_console_string[5];
                    argA[2] = asm_console_string[6];
                    argA[3] = asm_console_string[7];
                }
                
                
                //
                // Disassemble the opcode arguments
                //
                
                
                //
                // Set argument A
                
                if (argCount == 1) {
                    uint8_t regIndex = 0;
                    
                    if ((argA[0] == 'a') & (argA[1] == 'x')) {regIndex = 0;}
                    if ((argA[0] == 'b') & (argA[1] == 'x')) {regIndex = 1;}
                    if ((argA[0] == 'c') & (argA[1] == 'x')) {regIndex = 2;}
                    if ((argA[0] == 'd') & (argA[1] == 'x')) {regIndex = 3;}
                    if ((argA[0] == 'e') & (argA[1] == 'x')) {regIndex = 4;}
                    if ((argA[0] == 'f') & (argA[1] == 'x')) {regIndex = 5;}
                    if ((argA[0] == 'g') & (argA[1] == 'x')) {regIndex = 6;}
                    if ((argA[0] == 'h') & (argA[1] == 'x')) {regIndex = 7;}
                    
                    //
                    // Argument A is hex
                    
                    if (argAIsHex == 1) {
                        uint8_t hexValFlip[2] = {argA[1], argA[0]};
                        
                        uint8_t value = string_get_hex_char(hexValFlip);
                        
                        regIndex = value;
                    }
                    
                    fileBuffer[assemblyAddress - 1] = regIndex;
                    
                }
                
                
                //
                // Set argument A & B
                
                if (argCount == 2) {
                    uint8_t regIndexA = 0;
                    uint8_t regIndexB = 0;
                    
                    if ((argA[0] == 'a') & (argA[1] == 'x')) {regIndexA = 0;}
                    if ((argA[0] == 'b') & (argA[1] == 'x')) {regIndexA = 1;}
                    if ((argA[0] == 'c') & (argA[1] == 'x')) {regIndexA = 2;}
                    if ((argA[0] == 'd') & (argA[1] == 'x')) {regIndexA = 3;}
                    if ((argA[0] == 'e') & (argA[1] == 'x')) {regIndexA = 4;}
                    if ((argA[0] == 'f') & (argA[1] == 'x')) {regIndexA = 5;}
                    if ((argA[0] == 'g') & (argA[1] == 'x')) {regIndexA = 6;}
                    if ((argA[0] == 'h') & (argA[1] == 'x')) {regIndexA = 7;}
                    
                    if ((argB[0] == 'a') & (argB[1] == 'x')) {regIndexB = 0;}
                    if ((argB[0] == 'b') & (argB[1] == 'x')) {regIndexB = 1;}
                    if ((argB[0] == 'c') & (argB[1] == 'x')) {regIndexB = 2;}
                    if ((argB[0] == 'd') & (argB[1] == 'x')) {regIndexB = 3;}
                    if ((argB[0] == 'e') & (argB[1] == 'x')) {regIndexB = 4;}
                    if ((argB[0] == 'f') & (argB[1] == 'x')) {regIndexB = 5;}
                    if ((argB[0] == 'g') & (argB[1] == 'x')) {regIndexB = 6;}
                    if ((argB[0] == 'h') & (argB[1] == 'x')) {regIndexB = 7;}
                    
                    //
                    // Argument B is hex
                    
                    if (argBIsHex == 1) {
                        
                        // Move reg should be move byte
                        if (fileBuffer[assemblyAddress - 3] == 0x88) {
                            
                            fileBuffer[assemblyAddress - 3] = 0x89;
                            
                            uint8_t hexValFlip[2] = {argB[1], argB[0]};
                            
                            uint8_t value = string_get_hex_char(hexValFlip);
                            
                            regIndexB = value;
                            
                        }
                        
                    }
                    
                    fileBuffer[assemblyAddress - 2] = regIndexA;
                    fileBuffer[assemblyAddress - 1] = regIndexB;
                    
                }
                
                
                //
                // Set argument A as an address
                
                if (argCount == 4) {
                    
                    union Pointer ptr;
                    
                    ptr.address = string_get_int_long(argA);
                    
                    fileBuffer[assemblyAddress - 4] = ptr.byte_t[3];
                    fileBuffer[assemblyAddress - 3] = ptr.byte_t[2];
                    fileBuffer[assemblyAddress - 2] = ptr.byte_t[1];
                    fileBuffer[assemblyAddress - 1] = ptr.byte_t[0];
                    
                }
                
                
                // Clear the old op codes
                for (uint8_t i=0; i < 4; i++) 
                    opCode[i] = ' ';
                
                for (uint8_t i; i < 40; i++) 
                    asm_console_string[i] = ' ';
                
                printAddressLine();
                
                asm_console_string_length = 0;
                
            }
            
            // Clear the old op codes
            for (uint8_t i=0; i < 4; i++) {
                asm_console_string[i] = ' ';
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
            
            // Check to clear the old op codes
            if (asm_console_string_length == 0) {
                
                for (uint8_t i=0; i < 4; i++) {
                    asm_console_string[i] = ' ';
                }
                
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
        if (lastChar == 0x07) {
            return;
        }
        
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
