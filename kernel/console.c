#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>
#include <kernel/bus/bus.h>

#include <kernel/console.h>

#include <drivers/keyboard/ps2/main.h>
#include <drivers/display/LiquidCrystalDisplayController/main.h>

#define CONSOLE_STRING_LENGTH  40

uint8_t console_string[CONSOLE_STRING_LENGTH];
uint8_t console_string_length = 0;

uint8_t console_position = 0;
uint8_t console_line     = 0;

uint8_t oldScanCodeLow = 0;
uint8_t oldScanCodeHigh = 0;

uint8_t lastChar = 0x00;

uint8_t console_prompt[8];
uint8_t console_prompt_length = 1;

struct Driver* displayDevice;
struct Driver* keyboadDevice;

#define CONSOLE_FUNCTION_NAME_LENGTH   8

struct ConsoleCommand {
    
    uint8_t name[CONSOLE_FUNCTION_NAME_LENGTH];
    
    void(*function)(uint8_t* string, uint8_t length);
    
    
};


#define CONSOLE_FUNCTION_TABLE_SIZE  32

struct ConsoleCommand CommandRegistry[CONSOLE_FUNCTION_TABLE_SIZE];





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
    
    lastChar = decodeScanCode(oldScanCodeLow, oldScanCodeHigh);
    
    for (uint8_t i=0; i < CONSOLE_STRING_LENGTH; i++) 
        console_string[i] = ' ';
    
    displayDevice->write( SET_CURSOR_BLINK_RATE, 0x24 );
	
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

void consoleUpdate(void) {
    
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
    
    uint8_t scanCode = decodeScanCode(scanCodeLow, scanCodeHigh);
    
    if (scanCode == 0x00) 
        return;
    
    if (lastChar == scanCode) 
        return;
    
    lastChar = scanCode;
    
    
    //
    // Backspace
    //
    
    if (scanCode == 0x01) {
        
        if (console_string_length > 0) {
            
            // Remove last character from the console string
            console_string[ console_string_length - 1 ] = ' ';
            
            // Remove the character from the display
            displayDevice->write( console_string_length + (20 * console_line) + console_prompt_length - 1, ' ' );
            
            // Decrement the console string length
            console_string_length--;
            
        }
        
    }
    
    
    //
    // Return
    //
    
    if (scanCode == 0x02) {
        
        printPrompt();
        
        printLn();
        
        // Look up function name
        
        uint8_t isRightFunction = 0;
        uint8_t parameters_begin = 0;
        
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
            
            
            uint8_t length = console_string_length;
            
            console_string_length = 0;
            
            console_position = 0;
            
            
            // Run the function
            if (CommandRegistry[i].function != nullptr) 
                CommandRegistry[i].function( &console_string[parameters_begin], length - parameters_begin );
            
            break;
        }
        
        if ((isRightFunction == 0) & (console_string_length > 0)) {
            
            uint8_t badCommandOrFilename[] = "Bad cmd or filename";
            
            print( badCommandOrFilename, sizeof(badCommandOrFilename) );
            
            printLn();
            
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
        
        console_string[console_string_length] = scanCode;
        
        console_string_length++;
        
        console_position++;
        
        if (console_string_length >= 20) 
            console_string_length = 20;
        
    }
    
    printPrompt();
    
    uint8_t promptOffset = console_prompt_length;
    
    for (uint8_t i=0; i < console_string_length; i++) 
        displayDevice->write( i + (20 * console_line) + promptOffset, console_string[i] );
    
    displayDevice->write( SET_CURSOR_POSITION, console_string_length + promptOffset);
    
    displayDevice->write( SET_CURSOR_LINE, console_line);
    
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
    
    console_string_length = 0;
    
    return;
}

void printLn(void) {
    
    // Move down a line
    if (console_line < 3) {
        
        console_line++;
        
        displayDevice->write( SET_CURSOR_LINE, console_line);
        
    } else {
        
        // Otherwise shift the frame down
        displayDevice->write( SHIFT_FRAME_DOWN, 0x01);
        
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
    
    displayDevice->write( SET_CURSOR_POSITION, console_position );
    
    return;
}

void ConsoleSetBlinkRate(uint8_t rate) {
    
    displayDevice->write( SET_CURSOR_BLINK_RATE, rate);
    
    return;
}

void ConsoleSetCursor(uint8_t line, uint8_t position) {
    
    console_line = line;
    
    console_position = position;
    
    return;
}

void ConsoleSetPrompt(uint8_t* prompt, uint8_t length) {
    
    for (uint8_t i=0; i < length; i++) 
        console_prompt[i] = prompt[i];
    
    console_prompt_length = length - 1;
    
    return;
}

