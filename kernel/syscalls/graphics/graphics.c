#include <kernel/syscalls/graphics/graphics.h>

extern struct Driver* displayDevice;

void swapBuffers(void) {
    displayDevice->write(0x00009, 1); // Trigger a buffer swap
    return;
}

int8_t glInit(void) {
    displayDevice->write(0x00000, 2); // Display - Sprite mode
    
    return 1;
}

void glClear(uint8_t character) {
    
    for (uint8_t l=0; l < 8; l++) {
        displayDevice->write(0x00001, l); // Line
        displayDevice->write(0x00002, 0); // Pos
        
        for (uint8_t c=0; c < 16; c++) 
            displayDevice->write( 0x0000a + c, c + (l * 16)); // Draw to sprite memory
    }
    
    return;
}
