#include <kernel/bitwise.h>


uint8_t FlipBits(uint8_t input) {
    
    uint8_t output = 0;
    
    if (input & 0b00000001) output += (1 << 7);
    if (input & 0b00000010) output += (1 << 6);
    if (input & 0b00000100) output += (1 << 5);
    if (input & 0b00001000) output += (1 << 4);
    if (input & 0b00010000) output += (1 << 3);
    if (input & 0b00100000) output += (1 << 2);
    if (input & 0b01000000) output += (1 << 1);
    if (input & 0b10000000) output += (1 << 0);
    
    return output;
}
