#include <kernel/kernel.h>

uint8_t BitGet(uint8_t byte, uint8_t bit);

void BitSet(uint8_t byte, uint8_t bit, uint8_t state);

void BitClear(uint8_t byte, uint8_t bit, uint8_t state);


uint8_t FlipBits(uint8_t input);

uint8_t InvertBits(uint8_t input);
