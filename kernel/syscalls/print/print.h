<<<<<<< HEAD
#ifndef __SYSCALL_PRINT_
#define __SYSCALL_PRINT_

#include <kernel/kernel.h>

void print(uint8_t* string, uint8_t length);
void printInt(uint32_t integer);
void printc(char* string);
void printChar(uint8_t character);
void printLn(void);
void printSpace(uint8_t numberOfSpaces);
void printPrompt(void);
uint8_t printPause(void);

#endif
=======
#ifndef __SYSCALL_PRINT_
#define __SYSCALL_PRINT_

#include <kernel/kernel.h>

void print(uint8_t* string, uint8_t length);
void printInt(uint32_t integer);
void printc(char* string);
void printChar(uint8_t character);
void printLn(void);
void printSpace(uint8_t numberOfSpaces);
void printPrompt(void);
uint8_t printPause(void);

#endif
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
