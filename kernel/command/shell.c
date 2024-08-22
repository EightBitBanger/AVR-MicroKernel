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

extern uint16_t displayRows;
extern uint16_t displayColumbs;

extern struct Driver* displayDevice;
extern struct Driver* keyboadDevice;

extern struct ConsoleCommand CommandRegistry[CONSOLE_FUNCTION_TABLE_SIZE];

uint8_t new_line = 0;


void cliRunShell(void) {
    
    // Check the current scan code
    uint8_t scanCode = ConsoleGetLastChar();
    
    if (scanCode == 0) 
        return;
    
    // Shift pressed
    if (scanCode == 0x11) 
        shiftState = 1;
    
    // Shift released
    if (scanCode == 0x12) 
        shiftState = 0;
    
    //
    // Return
    //
    
    if (scanCode == 0x02) {
        
        printLn();
        ConsoleSetCursorPosition(0);
        
        uint8_t isRightFunction = 0;
        uint8_t parameters_begin = 0;
        
        // Save last entered command string
        for (uint8_t a=0; a <= console_string_length; a++) 
            console_string_old[a] = console_string[a];
        
        console_string_length_old = console_string_length;
        
        //
        // Check special character functionality
        
        if ((console_string[1] == ':') & 
            (console_string[2] == ' ')) {
            
            uppercase(&console_string[0]);
            
            // Device letter
            if ((console_string[0] >= 'A') & 
                (console_string[0] <= 'Z')) {
                
                // Update the prompt if the device letter was changed
                
                fsSetDeviceLetter( console_string[0] );
                
                uint8_t consolePrompt[2];
                consolePrompt[0] = console_string[0];
                consolePrompt[1] = '>';
                
                ConsoleSetPrompt(consolePrompt, 3);
                
                ConsoleSetCursorPosition(2);
                ConsoleClearKeyboardString();
                
                fsClearWorkingDirectory();
                fsWorkingDirectorySetStack(0);
                
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
        
        // Check file exists in working directory
        uint32_t fileAddress = fsFileExists(filename, parameters_begin);
        
        if (fileAddress > 0) {
            
            // Check file is executable
            struct FSAttribute attribute;
            fsFileGetAttributes(fileAddress, &attribute);
            
            // Check executable
            if (attribute.executable == 'x') {
                
                fsFileOpen(fileAddress);
                
                union Pointer fileSizePtr;
                for (uint8_t i=0; i < 4; i++) 
                    fs_read_byte(fileAddress + FILE_OFFSET_SIZE + i, &fileSizePtr.byte_t[i]);
                
                uint32_t programSize = fileSizePtr.address;
                
                uint8_t programBuffer[programSize];
                
                // Load the file
                fsFileRead(programBuffer, programSize);
                
                fsFileClose();
                
                // Emulate the code
                EmulateX4(&programBuffer[0], programSize);
                
                isRightFunction = 1;
                
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
        
        ConsoleClearKeyboardString();
        
        console_string_length = 0;
        
        printPrompt();
        
        return;
    }
    
    
    //
    // Backspace
    //
    
    if (scanCode == 0x01) {
        
        if (console_string_length > 0) {
            
            // Remove last character from the console string
            console_string[ console_string_length - 1 ] = ' ';
            
            // Decrement the console string length
            console_string_length--;
            console_position--;
            
            displayDevice->write( 0x00002, console_position);
            displayDevice->write( 0x0000a, ' ');
            displayDevice->write( 0x00002, console_position);
            
        }
        
    }
    
    //
    // Add the character
    
    if (scanCode > 0x19) {
        
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
        
        console_string[console_string_length] = scanCode;
        
        displayDevice->write( 0x0000a, scanCode);
        
        console_string_length++;
        
        console_position++;
        displayDevice->write( 0x00002, console_position);
        
        if (console_position >= 21) {
            
            printLn();
            
            console_position = 0;
            displayDevice->write( 0x00002, console_position);
            
        }
        
        return;
    }
    
    return;
}

