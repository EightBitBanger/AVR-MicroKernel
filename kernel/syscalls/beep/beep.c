<<<<<<< HEAD
#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/syscalls/beep/beep.h>

void beep(uint16_t duration, uint16_t frequency) {
    
    struct Bus spkBus;
    spkBus.read_waitstate  = 1000;
    spkBus.write_waitstate = 1000;
    
    for (uint16_t i=0; i < duration; i++) {
        
        bus_write_byte(&spkBus, 0x60000, 0xff);
        
        for (uint16_t i=0; i < frequency / 16; i++) 
            __asm__("nop");
        
        continue;
    }
    
    return;
}

void sysbeep(void) {
    
    beep(300, 14000);
    
    return;
}
=======
#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/syscalls/beep/beep.h>

void beep(uint16_t duration, uint16_t frequency) {
    
    struct Bus spkBus;
    spkBus.read_waitstate  = 1000;
    spkBus.write_waitstate = 1000;
    
    for (uint16_t i=0; i < duration; i++) {
        
        bus_write_byte(&spkBus, 0x60000, 0xff);
        
        for (uint16_t i=0; i < frequency / 16; i++) 
            __asm__("nop");
        
        continue;
    }
    
    return;
}

void sysbeep(void) {
    
    beep(300, 14000);
    
    return;
}
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
