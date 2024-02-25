#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <kernel/console.h>

#include <drivers/keyboard/ps2/main.h>
#include <drivers/display/LiquidCrystalDisplayController/main.h>


uint8_t console_string[20];
uint8_t console_string_length = 0;

uint8_t console_position = 0;
uint8_t console_line     = 0;

uint8_t oldScanCodeLow = 0;
uint8_t oldScanCodeHigh = 0;

uint8_t lastChar = 0x00;


struct Driver* displayDevice;
struct Driver* keyboadDevice;

#define CONSOLE_FUNCTION_NAME_LENGTH   8

struct ConsoleCommand {
    
    uint8_t name[CONSOLE_FUNCTION_NAME_LENGTH];
    
    void(*function)();
    
    
};

struct ConsoleCommand CommandRegistry[10];





void consoleInitiate(void) {
    
	uint8_t nameKeyboard[] = "PS2";
	keyboadDevice = (struct Driver*)GetDriverByName( nameKeyboard, sizeof(nameKeyboard) );
	
	uint8_t nameString[] = "display";
	displayDevice = (struct Driver*)GetDriverByName( nameString, sizeof(nameString) );
	
    keyboadDevice->read( 0x00000, &oldScanCodeLow );
    keyboadDevice->read( 0x00001, &oldScanCodeHigh );
    
    lastChar = decodeScanCode(oldScanCodeLow, oldScanCodeHigh);
    
    for (uint8_t i=0; i < 20; i++) 
        console_string[i] = ' ';
    
    displayDevice->write( SET_CURSOR_BLINK_RATE, 0x24 );
	
	for (uint8_t i=0; i < 10; i++) {
        
        for (uint8_t n=0; n < CONSOLE_FUNCTION_NAME_LENGTH; n++) {
            CommandRegistry[i].name[n] = ' ';
        }
        
        CommandRegistry[i].function = nullptr;
        
	}
	
	return;
}

void consoleUpdate(void) {
    
    uint8_t scanCodeLow  = 0;
    uint8_t scanCodeHigh = 0;
    
    keyboadDevice->read( 0x00000, &scanCodeLow );
    keyboadDevice->read( 0x00001, &scanCodeHigh );
    
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
    
    // Backspace
    if (scanCode == 0x01) {
        
        if (console_string_length > 0) {
            
            console_string[ console_string_length - 1 ] = ' ';
            
            displayDevice->write( (console_string_length - 1) + (20 * console_line), ' ' );
            
            console_string_length--;
            
            displayDevice->write( SET_CURSOR_POSITION, console_string_length);
            
            displayDevice->write( SET_CURSOR_LINE, console_line);
            
        }
        
    }
    
    
    // Return
    if (scanCode == 0x02) {
        
        printLn();
        
        if (console_string_length == 0) 
            return;
        
        // Look up function name
        
        uint8_t isRightFunction = 0;
        
        for (uint8_t i=0; i < 10; i++) {
            
            isRightFunction = 1;
            
            for (uint8_t n=0; n < CONSOLE_FUNCTION_NAME_LENGTH; n++) {
                
                if (CommandRegistry[i].name[n] != console_string[n]) {
                    
                    isRightFunction = 0;
                    
                    break;
                }
                
            }
            
            if (isRightFunction == 0)
                continue;
            
            console_string_length = 0;
            
            console_position = 0;
            
            // Run the function
            if (CommandRegistry[i].function != nullptr) 
                CommandRegistry[i].function();
            
            break;
        }
        
        for (uint8_t i=0; i < 20; i++) 
            console_string[i] = ' ';
        
        console_string_length = 0;
        
        console_position = 0;
        
        displayDevice->write( SET_CURSOR_POSITION, console_position);
        
        displayDevice->write( SET_CURSOR_LINE, console_line);
        
    }
    
    
    // Character
    if (scanCode > 0x29) {
        
        console_string[console_string_length] = scanCode;
        
        console_string_length++;
        
        if (console_string_length >= 20) 
            console_string_length = 20;
        
        displayDevice->write( SET_CURSOR_POSITION, console_string_length);
        
    }
    
    for (uint8_t i=0; i < console_string_length; i++) 
        displayDevice->write( 0x00000 + i + (20 * console_line), console_string[i] );
    
    return;
}


uint8_t ConsoleCommandRegister(uint8_t* name, uint8_t nameLength, void(*functionPtr)()) {
    
    uint8_t index = 0;
    for (uint8_t i=0; i < 10; i++) {
        
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
        
        _delay_ms(80);
        
    }
    
    return;
}

void ConsoleSetCursor(uint8_t line, uint8_t position) {
    
    console_line = line;
    
    console_position = position;
    
    return;
}
