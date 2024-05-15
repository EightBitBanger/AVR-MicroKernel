#ifndef __CONSOLE_INTERFACE_
#define __CONSOLE_INTERFACE_

#define CURSOR_BLINK_RATE  35

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>
#include <kernel/driver/driver.h>

#include <kernel/cstring.h>

#define  MSG_DEVICE_NOT_READY   1

void consoleInitiate(void);

void consoleRunShell(void);

uint8_t ConsoleRegisterCommand(uint8_t* name, uint8_t nameLength, void(*functionPtr)(uint8_t* string, uint8_t length));

uint8_t ConsoleGetRawChar(void);
uint8_t ConsoleGetLastChar(void);

void print(uint8_t* string, uint8_t length);
void printChar(uint8_t character);
void printLn(void);
void printSpace(uint8_t numberOfSpaces);
void printPrompt(void);
void printMessage(uint8_t index);

void ConsoleSetCursor(uint8_t line, uint8_t position);
void ConsoleCursorEnable(void);
void ConsoleCursorDisable(void);
void ConsoleSetCursorPosition(uint8_t position);
void ConsoleSetBlinkRate(uint8_t rate);
void ConsoleSetPrompt(uint8_t* prompt, uint8_t length);
void ConsoleClearScreen(void);

uint8_t consoleWait(uint8_t key);

#endif
