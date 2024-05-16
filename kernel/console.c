#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/console.h>

#include <kernel/kernel.h>
#include <kernel/bus/bus.h>

#include <drivers/keyboard/ps2/main.h>

#define CONSOLE_STRING_LENGTH  40

uint16_t display_width  = 0;
uint16_t display_height = 0;

uint8_t console_string[CONSOLE_STRING_LENGTH];
uint8_t console_string_old[CONSOLE_STRING_LENGTH];
uint8_t console_string_length     = 0;
uint8_t console_string_length_old = 0;

uint8_t console_position = 0;
uint8_t console_line     = 0;

uint8_t oldScanCodeLow  = 0;
uint8_t oldScanCodeHigh = 0;

uint8_t lastChar = 0x00;

uint8_t console_prompt[8];
uint8_t console_prompt_length = 1;

uint8_t cursor_blink_rate = 35;

struct Driver* displayDevice;
struct Driver* keyboadDevice;

#define CONSOLE_FUNCTION_NAME_LENGTH   10


uint8_t msgDeviceNotReady[]    = "Device not ready";

void printMessage(uint8_t index) {
    
    if (index == MSG_DEVICE_NOT_READY) print(msgDeviceNotReady, sizeof(msgDeviceNotReady));
    
    return;
}


struct ConsoleCommand {
    
    uint8_t name[CONSOLE_FUNCTION_NAME_LENGTH];
    
    void(*function)(uint8_t* string, uint8_t length);
    
};


#define CONSOLE_FUNCTION_TABLE_SIZE  32

struct ConsoleCommand CommandRegistry[CONSOLE_FUNCTION_TABLE_SIZE];


uint8_t consoleWait(uint8_t key) {
    
#ifdef BOARD_RETRO_AVR_X4_REV1
    keyboadDevice->read( 0x00001, &oldScanCodeLow );
    keyboadDevice->read( 0x00000, &oldScanCodeHigh );
#endif
    
#ifdef BOARD_RETROBOARD_REV2
    keyboadDevice->read( 0x00000, &oldScanCodeLow );
    keyboadDevice->read( 0x00001, &oldScanCodeHigh );
#endif
    
    uint8_t currentChar = kbDecodeScanCode(oldScanCodeLow, oldScanCodeHigh);
    currentChar = lastChar;
    
    ConsoleSetCursor(console_line, 0);
    
    while (currentChar == lastChar) {
        
#ifdef BOARD_RETRO_AVR_X4_REV1
        keyboadDevice->read( 0x00001, &oldScanCodeLow );
        keyboadDevice->read( 0x00000, &oldScanCodeHigh );
#endif
        
#ifdef BOARD_RETROBOARD_REV2
        keyboadDevice->read( 0x00000, &oldScanCodeLow );
        keyboadDevice->read( 0x00001, &oldScanCodeHigh );
#endif
        
        currentChar = kbDecodeScanCode(oldScanCodeLow, oldScanCodeHigh);
        
        if (currentChar == 0x00) 
            lastChar = currentChar;
        
        continue;
    }
    
    lastChar = currentChar;
    
    if (key == lastChar) 
        return 1;
    
    return 0;
}


void consoleInitiate(void) {
    
    uint8_t nameKeyboard[] = "PS2";
	keyboadDevice = (struct Driver*)GetDriverByName( nameKeyboard, sizeof(nameKeyboard) );
	
	uint8_t nameDisplay[] = "display";
	displayDevice = (struct Driver*)GetDriverByName( nameDisplay, sizeof(nameDisplay) );
	
#ifdef BOARD_RETRO_AVR_X4_REV1
    keyboadDevice->read( 0x00001, &oldScanCodeLow );
    keyboadDevice->read( 0x00000, &oldScanCodeHigh );
#endif
    
#ifdef BOARD_RETROBOARD_REV2
    keyboadDevice->read( 0x00000, &oldScanCodeLow );
    keyboadDevice->read( 0x00001, &oldScanCodeHigh );
#endif
    
    lastChar = kbDecodeScanCode(oldScanCodeLow, oldScanCodeHigh);
    
    for (uint8_t i=0; i < CONSOLE_STRING_LENGTH; i++) 
        console_string[i] = ' ';
    
    displayDevice->write( 163, 0x24 );
	
	for (uint8_t i=0; i < CONSOLE_FUNCTION_TABLE_SIZE; i++) {
        
        for (uint8_t n=0; n < CONSOLE_FUNCTION_NAME_LENGTH; n++) {
            CommandRegistry[i].name[n] = ' ';
        }
        
        CommandRegistry[i].function = nullptr;
        
	}
	
	uint8_t promptString[] = "A>";
	
	ConsoleSetPrompt( promptString, sizeof(promptString) );
	
	return;
}

uint8_t ConsoleGetRawChar(void) {
    
    uint8_t scanCodeLow  = 0;
    uint8_t scanCodeHigh = 0;
    
#ifdef BOARD_RETRO_AVR_X4_REV1
    keyboadDevice->read( 0x00001, &scanCodeLow );
    keyboadDevice->read( 0x00000, &scanCodeHigh );
#endif
    
#ifdef BOARD_RETROBOARD_REV2
    keyboadDevice->read( 0x00000, &scanCodeLow );
    keyboadDevice->read( 0x00001, &scanCodeHigh );
#endif
    
    return kbDecodeScanCode(scanCodeLow, scanCodeHigh);
}

uint8_t ConsoleGetLastChar(void) {
    
    uint8_t scanCodeLow  = 0;
    uint8_t scanCodeHigh = 0;
    
#ifdef BOARD_RETRO_AVR_X4_REV1
    keyboadDevice->read( 0x00001, &scanCodeLow );
    keyboadDevice->read( 0x00000, &scanCodeHigh );
#endif
    
#ifdef BOARD_RETROBOARD_REV2
    keyboadDevice->read( 0x00000, &scanCodeLow );
    keyboadDevice->read( 0x00001, &scanCodeHigh );
#endif
    
    if (oldScanCodeLow != scanCodeLow) lastChar = 0x00;
    if (oldScanCodeHigh != scanCodeHigh) lastChar = 0x00;
    
    oldScanCodeLow  = scanCodeLow;
    oldScanCodeHigh = scanCodeHigh;
    
    uint8_t scanCode = kbDecodeScanCode(scanCodeLow, scanCodeHigh);
    
    if (scanCode == 0x00) 
        return 0x00;
    
    if (lastChar == scanCode) 
        return 0x00;
    
    lastChar = scanCode;
    
    return lastChar;
}

void consoleRunShell(void) {
    
    while(1) {
        
        // Check the current scan code
        uint8_t scanCode = ConsoleGetLastChar();
        
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
            
            uint8_t isRightFunction = 0;
            uint8_t parameters_begin = 0;
            
            uint8_t length = console_string_length - parameters_begin;
            
            // Look up function name
            for (uint8_t i=0; i < CONSOLE_FUNCTION_TABLE_SIZE; i++) {
                
                isRightFunction = 1;
                
                for (uint8_t n=0; n < CONSOLE_FUNCTION_NAME_LENGTH; n++) {
                    
                    if (CommandRegistry[i].name[n] != console_string[n]) {
                        
                        isRightFunction = 0;
                        
                        break;
                    }
                    
                    if (parameters_begin == 0) {
                        
                        if (CommandRegistry[i].name[n] == ' ') {
                            
                            parameters_begin = n + 1;
                            
                            break;
                        }
                        
                    }
                    
                }
                
                if (isRightFunction == 0)
                    continue;
                
                // Save last entered command string
                for (uint8_t i=0; i < console_string_length; i++) 
                    console_string_old[i] = console_string[i];
                console_string_length_old = console_string_length;
                
                console_string_length = 0;
                
                console_position = 0;
                
                // Run the function
                if (CommandRegistry[i].function != nullptr) 
                    CommandRegistry[i].function( &console_string[parameters_begin], length );
                
                printPrompt();
                
                break;
            }
            
            // Check executable file exists
            uint8_t doesFileExists = fsFileExists(&console_string[parameters_begin], length);
            
            // Execute the file
            if (doesFileExists == 1) {
                
                
                uint8_t badCommandOrFilename[] = "Bad cmd or filename";
                print( badCommandOrFilename, sizeof(badCommandOrFilename) );
                
                printLn();
                
                printPrompt();
                
                return;
            }
            
            // Bad command for filename
            if ((doesFileExists == 0) & (isRightFunction == 0) & (console_string_length > 0)) {
                
                // Save last entered command string
                for (uint8_t i=0; i < console_string_length; i++) 
                    console_string_old[i] = console_string[i];
                console_string_length_old = console_string_length;
                
                ConsoleSetCursor(console_line, 0);
                
                uint8_t badCommandOrFilename[] = "Bad cmd or filename";
                print( badCommandOrFilename, sizeof(badCommandOrFilename) );
                
                printLn();
                
                printPrompt();
                
            }
            
            // Clear the console string
            for (uint8_t i=0; i < CONSOLE_STRING_LENGTH; i++) 
                console_string[i] = ' ';
            
            console_string_length = 0;
            
        }
        
        
        //
        // Add letter or number
        //
        
        if (scanCode > 0x19) {
            
            if (console_string_length < CONSOLE_STRING_LENGTH) {
                
                console_string[console_string_length] = scanCode;
                
                console_string_length++;
                
                
                ConsoleSetCursor(console_line, console_position + 1);
                console_position--;
                
                printChar( scanCode );
                
                if (console_position > 19) {
                    
                    if (console_line < 3) {
                        
                        printLn();
                        
                        console_position = 0;
                        
                        ConsoleSetCursor(console_line, console_position);
                        
                    } else {
                        
                        ConsoleSetCursor(console_line, 0);
                        
                        console_position = 0;
                        
                        console_line = 3;
                        
                        ConsoleSetCursor(console_line, console_position);
                        
                        printLn();
                        
                    }
                    
                }
                
                
            }
            
        }
        
    }
    
    return;
}


uint8_t ConsoleRegisterCommand(uint8_t* name, uint8_t nameLength, void(*functionPtr)(uint8_t* string, uint8_t length)) {
    
    uint8_t index = 0;
    for (uint8_t i=0; i < CONSOLE_FUNCTION_TABLE_SIZE; i++) {
        
        // Is set to a valid value
        if ((is_letter( &CommandRegistry[i].name[0] ) == 1) | 
            (is_number( &CommandRegistry[i].name[0] ) == 1))
            continue;
        
        index = i;
        
        break;
    }
    
    if (nameLength > CONSOLE_FUNCTION_NAME_LENGTH) 
        nameLength = CONSOLE_FUNCTION_NAME_LENGTH;
    
    for (uint8_t i=0; i < nameLength - 1; i++) 
        CommandRegistry[index].name[i] = name[i];
    
    CommandRegistry[index].function = functionPtr;
	
	return 1;
}


void print(uint8_t* string, uint8_t length) {
    
    for (uint8_t i=0; i < length - 1; i++) {
        
        displayDevice->write( console_position + (20 * console_line) + i, string[i] );
        
        continue;
    }
    
    console_position += length - 1;
    
    ConsoleSetCursorPosition(console_position);
    
    return;
}

void printChar(uint8_t character) {
    
    displayDevice->write( console_position + (20 * console_line), character );
    
    console_position++;
    
    return;
}

void printLn(void) {
    
    // Move down a line
    if (console_line < 3) {
        
        console_line++;
        
        displayDevice->write( 160, console_line);
        
    } else {
        
        // Otherwise shift the frame down
        displayDevice->write( 169, 0x01);
        
        _delay_ms(40);
        
    }
    
    console_position = 0;
    
    return;
}

void printSpace(uint8_t numberOfSpaces) {
    
    for (uint8_t i=0; i < numberOfSpaces; i++) {
        
        displayDevice->write( console_position + (20 * console_line), ' ' );
        
        console_position++;
    }
    
    return;
}

void printPrompt(void) {
    
    // Print a prompt char
    for (uint8_t i=0; i < console_prompt_length; i++) 
        displayDevice->write( (20 * console_line) + i, console_prompt[i] );
    
    console_position = console_prompt_length;
    
    displayDevice->write( 161, console_position );
    
    return;
}

void ConsoleSetBlinkRate(uint8_t rate) {
    
    cursor_blink_rate = rate;
    
    displayDevice->write( 163, rate);
    
    return;
}

void ConsoleSetCursor(uint8_t line, uint8_t position) {
    
    console_line = line;
    
    console_position = position;
    
    displayDevice->write( 160, console_line );
    displayDevice->write( 161, console_position );
    
    return;
}

void ConsoleSetCursorPosition(uint8_t position) {
    
    console_position = position;
    
    displayDevice->write( 161, console_position );
    
    return;
}

void ConsoleCursorEnable(void) {
    
    displayDevice->write( 163, cursor_blink_rate);
    
    return;
}

void ConsoleCursorDisable(void) {
    
    displayDevice->write( 163, 0);
    
    return;
}

void ConsoleSetPrompt(uint8_t* prompt, uint8_t length) {
    
    for (uint8_t i=0; i < length; i++) 
        console_prompt[i] = prompt[i];
    
    console_prompt_length = length - 1;
    
    return;
}

void ConsoleClearScreen(void) {
    
    // Clear display frame buffer
    displayDevice->write( 165, 0x01 );
    
    _delay_ms(40);
    
    return;
}

uint8_t ConsoleGetCursorPosition(void) {
    return console_position;
}

uint8_t ConsoleGetCursorLine(void) {
    return console_line;
}

