#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/syscalls/switch/switch.h>

uint8_t getSwitch(void) {
    
    struct Bus switchBus;
    switchBus.read_waitstate  = 2;
    switchBus.write_waitstate = 2;
    
    uint8_t switchState=0;
    bus_read_io(&switchBus, 0x00001, &switchState);
    
    return InvertBits( FlipBits( switchState ) );
}

