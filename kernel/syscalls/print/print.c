#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/command/cli.h>
#include <kernel/syscalls/print/print.h>

extern uint8_t console_prompt[16];
extern uint8_t console_prompt_length;

extern uint8_t console_position;
extern uint8_t console_line;

extern uint8_t console_prompt_length;

extern uint16_t displayRows;
extern uint16_t displayColumbs;

extern struct Driver* displayDevice;
extern struct Driver* keyboadDevice;


void print(uint8_t* string, uint8_t length) {
    
    displayDevice->write( 0x00001, console_line );
    
    for (uint8_t i=0; i < length - 1; i++) {
        
        displayDevice->write( 0x0000a + i, string[i] );
        
        console_position++;
    }
    swapBuffers();
    
    return;
}

void printInt(uint32_t integer) {
    
    uint8_t string[10];
    
    uint8_t place = int_to_string(integer, string);
    
    // Zero special case
    if (integer == 0) {
        
        printChar('0');
        
        return;
    }
    
    if (place == 0) 
        place++;
    
    displayDevice->write( 0x00001, console_line );
    displayDevice->write( 0x00002, console_position );
    
    for (uint8_t i=0; i < place; i++) 
        displayDevice->write( 0x0000a + i, string[i] );
    
    console_position += (place / 2) + 1;
    ConsoleSetCursorPosition(console_position);
    swapBuffers();
    
    return;
}

void printc(char* string, uint8_t length) {
    
    displayDevice->write( 0x00001, console_line );
    
    for (uint8_t i=0; i < length - 1; i++) {
        
        displayDevice->write( 0x00002, console_position );
        
        displayDevice->write( 0x0000a, string[i] );
        
        console_position++;
        
        continue;
    }
    
    ConsoleSetCursorPosition(console_position);
    swapBuffers();
    
    return;
}

void printChar(uint8_t character) {
    
    displayDevice->write( 0x00001, console_line );
    displayDevice->write( 0x00002, console_position );
    
    displayDevice->write( 0x0000a, character );
    
    console_position++;
    swapBuffers();
    
    return;
}

void printLn(void) {
    
    if (console_line < (displayRows - 1)) {
        
        console_line++;
        
        displayDevice->write( 0x00001, console_line);
        
    } else {
        
        displayDevice->write( 0x00005, 1);
        
        _delay_ms(30);
        
    }
    
    console_position = 0;
    
    ConsoleSetCursor(console_line, console_position);
    swapBuffers();
    
    return;
}

void printSpace(uint8_t numberOfSpaces) {
    displayDevice->write( 0x00001, console_line );
    
    for (uint8_t i=0; i < numberOfSpaces; i++) {
        displayDevice->write( 0x00002, console_position );
        displayDevice->write( 0x0000a + i, ' ' );
        console_position++;
    }
    
    ConsoleSetCursorPosition(console_position);
    swapBuffers();
    return;
}

void printPrompt(void) {
    displayDevice->write( 0x00001, console_line );
    displayDevice->write( 0x00002, 0 );
    
    for (uint8_t i=0; i < console_prompt_length - 1; i++) 
        displayDevice->write( 0x0000a + i, console_prompt[i] );
    
    console_position = console_prompt_length - 1;
    
    ConsoleSetCursorPosition(console_position);
    swapBuffers();
    return;
}

uint8_t printPause(void) {
    uint8_t msgPressAnyKey[]   = "Press any key...";
    print(msgPressAnyKey, sizeof(msgPressAnyKey));
    
    ConsoleSetCursorPosition( sizeof(msgPressAnyKey) - 1 );
    
    uint8_t keypress = ConsoleWait();
    
    ConsoleSetCursorPosition(0);
    
    for (uint8_t a=0; a < sizeof(msgPressAnyKey); a++) 
        printChar(' ');
    
    ConsoleSetCursorPosition(0);
    return keypress;
}

