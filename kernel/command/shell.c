#include <avr/io.h>

#include <kernel/delay.h>

#include <kernel/command/cli.h>

extern uint32_t fs_working_directory_address;

extern uint16_t display_width;
extern uint16_t display_height;

extern uint8_t console_string[CONSOLE_STRING_LENGTH];
extern uint8_t console_string_old[CONSOLE_STRING_LENGTH];
extern uint8_t console_string_length;
extern uint8_t console_string_length_old;

extern uint8_t console_position;
extern uint8_t console_line;

extern uint8_t oldScanCodeLow;
extern uint8_t oldScanCodeHigh;

extern uint8_t lastChar;

extern uint8_t shiftState;

extern uint8_t console_prompt[16];
extern uint8_t console_prompt_length;

extern uint8_t cursor_blink_rate;

extern struct Driver* displayDevice;
extern struct Driver* keyboadDevice;

extern struct ConsoleCommand CommandRegistry[CONSOLE_FUNCTION_TABLE_SIZE];


void cliRunShell(void) {
    
    // Check the current scan code
    uint8_t scanCode = ConsoleGetLastChar();
    
    if (scanCode == 0) 
        return;
    
    //
    // Shift state
    //
    
    // Shift pressed
    if (scanCode == 0x11) {
        shiftState = 1;
    }
    
    // Shift released
    if (scanCode == 0x12) {
        shiftState = 0;
    }
    
    //
    // Function F3 - Repeat last command
    //
    
    if (scanCode == 0xf3) {
        
        console_string_length = console_string_length_old;
        ConsoleSetCursorPosition(console_prompt_length);
        
        for (uint8_t i=0; i < console_string_length; i++) {
            console_string[i] = console_string_old[i];
            printChar( console_string[i] );
        }
        
        ConsoleSetCursorPosition( console_string_length + 2 );
        
        return;
    }
    
    
    //
    // Backspace
    //
    
    if (scanCode == 0x01) {
        
        if (console_string_length > 0) {
            
            if (console_position == 0) {
                console_line--;
                console_position = 20;
            }
            
            // Remove last character from the console string
            console_string[ console_string_length - 1 ] = ' ';
            
            // Remove the character from the display
            displayDevice->write( console_position + (20 * console_line) - 1, ' ' );
            
            // Decrement the console string length
            console_string_length--;
            console_position--;
            
            ConsoleSetCursor(console_line, console_position);
            
        }
        
    }
    
    
    //
    // Return
    //
    
    if (scanCode == 0x02) {
        
        printLn();
        ConsoleSetCursorPosition(0);
        
        uint8_t isRightFunction = 0;
        uint8_t parameters_begin = 0;
        
        // Check special character functionality
        if ((console_string[1] == ':') & 
            (console_string[2] == ' ')) {
            
            uppercase(&console_string[0]);
            
            uint8_t isChanged = 0;
            
            // Device letter
            if ((console_string[0] >= 'A') & 
                (console_string[0] <= 'Z')) 
                isChanged = 1;
            
            // Update the prompt if the device letter was changed
            if (isChanged == 1) {
                
                fsSetDeviceLetter( console_string[0] );
                
                fsSetRootDirectory( console_string[0] );
                
                uint8_t consolePrompt[2];
                consolePrompt[0] = console_string[0];
                consolePrompt[1] = '>';
                
                ConsoleSetPrompt(consolePrompt, 3);
                
                ConsoleSetCursorPosition(2);
                ConsoleClearKeyboardString();
                
                fsClearWorkingDirectory();
                fsSetDirectoryStack(0);
                
                console_string_length = 0;
                
            }
            
            printPrompt();
            
            return;
        }
        
        
        // Look up function name
        for (uint8_t i=0; i < CONSOLE_FUNCTION_TABLE_SIZE; i++) {
            
            isRightFunction = 1;
            parameters_begin = 0;
            
            for (uint8_t n=0; n < CONSOLE_FUNCTION_NAME_LENGTH; n++) {
                
                uint8_t consoleChar = console_string[n];
                lowercase(&consoleChar);
                
                if (CommandRegistry[i].name[n] != consoleChar) {
                    
                    isRightFunction = 0;
                    parameters_begin = 0;
                    
                    break;
                }
                
                if (CommandRegistry[i].name[n] == ' ') {
                    
                    if (parameters_begin == 0) 
                        parameters_begin = n + 1;
                    
                    break;
                }
                
                if (is_symbol(&console_string[n+1]) == 1) {
                    
                    if (parameters_begin == 0) 
                        parameters_begin = n + 1;
                    
                    break;
                }
                
                continue;
            }
            
            if (isRightFunction == 0)
                continue;
            
            // Save last entered command string
            for (uint8_t a=0; a <= console_string_length; a++) 
                console_string_old[a] = console_string[a];
            
            console_string_length_old = console_string_length;
            
            console_position = 0;
            
            // Run the function
            if (CommandRegistry[i].function != nullptr) 
                CommandRegistry[i].function( &console_string[parameters_begin], (console_string_length + 1) - parameters_begin );
            
            console_string_length = 0;
            
            printPrompt();
            
            ConsoleClearKeyboardString();
            
            break;
        }
        
        
        //
        // Check file executable
        
        // Get parameters
        uint8_t filename[CONSOLE_STRING_LENGTH];
        for (uint8_t i=0; i < CONSOLE_STRING_LENGTH; i++) 
            filename[i] = ' ';
        
        for (uint8_t i=0; i < CONSOLE_STRING_LENGTH; i++) {
            
            filename[i] = console_string[i];
            
            if (console_string[i] != ' ') 
                continue;
            
            parameters_begin = i + 1;
            break;
        }
        
        if (parameters_begin > FILE_NAME_LENGTH) 
            parameters_begin = FILE_NAME_LENGTH;
        
        // Are we currently in a working directory
        if (fsCheckWorkingDirectory() == 1) {
            
            // Check file exists in working directory
            uint32_t fileAddress = fsDirectoryFileExists(filename, parameters_begin);
            
            if (fileAddress > 0) {
                
                // Check file is executable
                struct FSAttribute attribute;
                fsGetFileAttributes(filename, parameters_begin, &attribute);
                
                // Check executable
                if (attribute.executable == 'x') {
                    uint8_t index = fsFileOpen(filename, parameters_begin);
                    
                    uint32_t programSize = fsGetFileSize(filename, FILE_NAME_LENGTH);
                    uint8_t programBuffer[programSize];
                    
                    if (index > 0) {
                        
                        // Load the file
                        fsFileReadBin(index, programBuffer, programSize);
                        
                        // Emulate the code
                        EmulateX4(programBuffer, programSize);
                        
                        fsFileClose(index);
                        
                        isRightFunction = 1;
                        
                    }
                    
                }
                
            }
            
        } else {
            
            
            // Execute from the root
            if (fsCheckDeviceReady() == 1) {
                
                uint32_t fileAddress = fsFileExists(filename, parameters_begin-1);
                
                if (fileAddress != 0) {
                    
                    uint8_t directoryFlag = 0;
                    fs_read_byte(fileAddress + OFFSET_DIRECTORY_FLAG, &directoryFlag);
                    
                    // Check file is on the root directory
                    if (directoryFlag == 0) {
                        
                        // Check file is executable
                        struct FSAttribute attribute;
                        fsGetFileAttributes(filename, parameters_begin, &attribute);
                        
                        // Check executable
                        if (attribute.executable == 'x') {
                            
                            uint8_t index = fsFileOpen(filename, parameters_begin);
                            
                            uint32_t programSize = fsGetFileSize(filename, FILE_NAME_LENGTH);
                            uint8_t programBuffer[programSize];
                            
                            if (index > 0) {
                                
                                // Load the file
                                fsFileReadBin(index, programBuffer, programSize);
                                
                                // Emulate the code
                                EmulateX4(programBuffer, programSize);
                                
                                fsFileClose(index);
                                
                                isRightFunction = 1;
                                
                            }
                            
                        }
                        
                    }
                    
                    
                }
                
            }
            
        }
        
        
        //
        // Bad command or filename
        
        if ((isRightFunction == 0) & (console_string_length > 0)) {
            
            ConsoleSetCursor(console_line, 0);
            
            uint8_t badCommandOrFilename[] = "Bad cmd or filename";
            print( badCommandOrFilename, sizeof(badCommandOrFilename) );
            
            printLn();
            
        }
        
        // Save last entered command string
        for (uint8_t i=0; i < console_string_length; i++) 
            console_string_old[i] = console_string[i];
        console_string_length_old = console_string_length;
        
        ConsoleClearKeyboardString();
        
        console_string_length = 0;
        
        printPrompt();
        
        return;
    }
    
    
    //
    // Add letter or number
    //
    
    if (scanCode > 0x19) {
        
        if (console_string_length < CONSOLE_STRING_LENGTH) {
            
            if (shiftState == 1) {
                
                if (is_letter(&scanCode) == 1) {
                    
                    scanCode -= 0x20;
                    
                } else {
                    
                    if ((scanCode == '[') | (scanCode == ']')) scanCode += 0x20;
                    
                    if (scanCode == 0x3B) scanCode--;
                    
                    if (scanCode == '-')  scanCode = '_';
                    if (scanCode == '=')  scanCode = '+';
                    if (scanCode == '\\') scanCode = '|';
                    if (scanCode == 0x27) scanCode = 0x22;
                    if (scanCode == ',')  scanCode = '<';
                    if (scanCode == '.')  scanCode = '>';
                    if (scanCode == '/')  scanCode = '?';
                    
                }
                
                if (is_number(&scanCode) == 1) {
                    
                    if (scanCode == '0') scanCode = ')';
                    if (scanCode == '1') scanCode = '!';
                    if (scanCode == '2') scanCode = '@';
                    if (scanCode == '3') scanCode = '#';
                    if (scanCode == '4') scanCode = '$';
                    if (scanCode == '5') scanCode = '%';
                    if (scanCode == '6') scanCode = '^';
                    if (scanCode == '7') scanCode = '&';
                    if (scanCode == '8') scanCode = '*';
                    if (scanCode == '9') scanCode = '(';
                    
                }
                
            }
            
            // Print the character
            console_string[console_string_length] = scanCode;
            
            console_string_length++;
            
            printChar(scanCode);
            
            
            
            if (ConsoleGetCursorPosition() > 19) {
                
                if (ConsoleGetCursorLine() < 3) {
                    
                    //printLn();
                    
                    //ConsoleSetCursor(console_line, console_position);
                    
                } else {
                    
                    //printLn();
                    
                    //ConsoleSetCursorPosition(0);
                    
                }
                
            }
            
            
        }
        
    }
    
    return;
}

