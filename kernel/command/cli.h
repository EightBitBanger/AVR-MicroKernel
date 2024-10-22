#ifndef __CONSOLE_COMMAND_LINE_INTERPRETER_
#define __CONSOLE_COMMAND_LINE_INTERPRETER_

#define CURSOR_BLINK_RATE  60
#define CONSOLE_STRING_LENGTH  40
#define CONSOLE_FUNCTION_NAME_LENGTH   11
#define CONSOLE_FUNCTION_TABLE_SIZE  32

#include <kernel/kernel.h>

#define  MSG_DEVICE_NOT_READY   1
#define  MSG_DEVICE_NOT_FOUND   2

struct ConsoleCommand {
    
    uint8_t name[CONSOLE_FUNCTION_NAME_LENGTH];
    
    void(*function)(uint8_t* string, uint8_t length);
    
};


void cliRunShell(void);

void cliInit(void);

uint8_t ConsoleRegisterCommand(uint8_t* name, uint8_t nameLength, void(*functionPtr)(uint8_t* string, uint8_t length));

uint8_t ConsoleGetRawChar(void);
uint8_t ConsoleGetLastChar(void);

void ConsoleSetCursor(uint8_t line, uint8_t position);
void ConsoleCursorEnable(void);
void ConsoleCursorDisable(void);
void ConsoleSetCursorPosition(uint8_t position);
void ConsoleSetBlinkRate(uint8_t rate);
void ConsoleSetPrompt(uint8_t* prompt, uint8_t length);
void ConsoleClearScreen(void);

uint8_t ConsoleGetCursorPosition(void);
uint8_t ConsoleGetCursorLine(void);

uint8_t ConsoleWait(void);
void ConsoleClearKeyboardString(void);

#endif
