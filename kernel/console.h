#ifndef __CONSOLE_INTERFACE_
#define __CONSOLE_INTERFACE_

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>
#include <kernel/driver/driver.h>

#include <kernel/cstring.h>

void consoleInitiate(void);

void consoleUpdate(void);

uint8_t ConsoleRegisterCommand(uint8_t* name, uint8_t nameLength, void(*functionPtr)(uint8_t* string, uint8_t length));

void print(uint8_t* string, uint8_t length);

void printLn(void);

void printSpace(uint8_t numberOfSpaces);

void printPrompt(void);

void ConsoleSetCursor(uint8_t line, uint8_t position);

void ConsoleSetPrompt(uint8_t* prompt, uint8_t length);


#endif
