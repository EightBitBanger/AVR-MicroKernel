#include <kernel/bitwise.h>

uint8_t BitGet(uint8_t byte, uint8_t bit) {
    return (byte >> bit) & 1;
}

void BitSet(uint8_t byte, uint8_t bit, uint8_t state) {
    if (state == 1) {byte |= (1 << bit);} else {byte &= ~(1 << bit);}
    return;
}

void BitClear(uint8_t byte, uint8_t bit, uint8_t state) {
    byte |= (1 << bit);
    return;
}



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

uint8_t InvertBits(uint8_t input) {
    
    uint8_t output = 0;
    
    if (BitGet(input, 0) != 1) {output += (1 << 0);}
    if (BitGet(input, 1) != 1) {output += (1 << 1);}
    if (BitGet(input, 2) != 1) {output += (1 << 2);}
    if (BitGet(input, 3) != 1) {output += (1 << 3);}
    if (BitGet(input, 4) != 1) {output += (1 << 4);}
    if (BitGet(input, 5) != 1) {output += (1 << 5);}
    if (BitGet(input, 6) != 1) {output += (1 << 6);}
    if (BitGet(input, 7) != 1) {output += (1 << 7);}
    
    return output;
}
