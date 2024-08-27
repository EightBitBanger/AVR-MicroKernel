#ifndef __SYSCALL_PRINT_
#define __SYSCALL_PRINT_

#include <kernel/kernel.h>

void print(uint8_t* string, uint8_t length);

void printInt(uint32_t integer);

void printc(char* string, uint8_t length);

void printChar(uint8_t character);

void printLn(void);

void printSpace(uint8_t numberOfSpaces);

void printPrompt(void);

uint8_t printPause(void);

#endif
