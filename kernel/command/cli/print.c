#include <avr/io.h>

#include <kernel/delay.h>

#include <kernel/command/cli/cli.h>

#include <kernel/kernel.h>
#include <kernel/bus/bus.h>

#include <drivers/keyboard/ps2/main.h>

extern uint8_t console_prompt[16];
extern uint8_t console_prompt_length;

extern uint8_t console_position;
extern uint8_t console_line;

extern uint8_t console_prompt_length;

extern struct Driver* displayDevice;
extern struct Driver* keyboadDevice;


void printk(uint8_t* string) {
    
    uint8_t pos = 0;
    uint8_t line = 0;
    
    for (uint16_t i=0; i < 1024; i++) {
        
        // Null terminate
        if (string[i] == '\0') {
            return;
        }
        
        // New line
        if (string[i] == '\n') {
            
            if (line < 3) {
                line++;
                displayDevice->write( 160, line);
            } else {
                // Otherwise shift the frame down
                displayDevice->write( 169, 0x01);
                _delay_ms(40);
            }
            
            pos = 0;
            
            continue;
        }
        
        displayDevice->write( pos + (20 * line), string[i] );
        
        pos++;
        
        continue;
    }
    
    ConsoleSetCursor(console_line, console_position);
    
    return;
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

void printInt(uint32_t integer) {
    
    uint8_t string[10];
    
    uint8_t place = int_to_string(integer, string);
    
    // Zero special case
    if (integer == 0) {
        
        printChar('0');
        
        return;
        
    } else {
        
        for (uint8_t i=0; i < place; i++) {
            
            displayDevice->write( console_position + (20 * console_line) + i, string[i] );
            
            continue;
        }
        
    }
    
    console_position += place;
    
    ConsoleSetCursorPosition(console_position);
    
    return;
}

void printc(char* string, uint8_t length) {
    
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
    
    if (console_position > 19) {
        console_position = 0;
        console_line++;
    }
    
    if (console_line > 3) {
        console_line = 3;
        printLn();
    }
    
    ConsoleSetCursorPosition(console_position);
    
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
    
    // Print prompt string
    for (uint8_t i=0; i < console_prompt_length - 1; i++) 
        displayDevice->write( (20 * console_line) + i, console_prompt[i] );
    
    console_position = console_prompt_length - 1;
    
    displayDevice->write( 161, console_position );
    
    return;
}

