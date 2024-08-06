#ifndef __PRINT_SYSCALL_
#define __PRINT_SYSCALL_

#include <kernel/kernel.h>

void printk(uint8_t* string);

void print(uint8_t* string, uint8_t length);

void printInt(uint32_t integer);

void printc(char* string, uint8_t length);

void printChar(uint8_t character);

void printLn(void);

void printSpace(uint8_t numberOfSpaces);

void printPrompt(void);

#endif
