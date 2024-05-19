#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/task.h>

void functionTASK(uint8_t* param, uint8_t param_length) {
    
    task_kill(param, param_length-1);
    
    return;
}

void registerCommandTASK(void) {
    
    uint8_t taskCommandName[] = "task";
    
    ConsoleRegisterCommand(taskCommandName, sizeof(taskCommandName), functionTASK);
    
    return;
}
