#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/task.h>

uint8_t msgTaskFound[]     = "Task stopped";
uint8_t msgTaskNotFound[]  = "Task not found";


void functionTASK(uint8_t* param, uint8_t param_length) {
    
    if (task_find(param, param_length) != 0) {
        
        print(msgTaskFound, sizeof(msgTaskFound));
        printLn();
        
        task_kill(param, param_length);
        
    } else {
        
        print(msgTaskNotFound, sizeof(msgTaskNotFound));
        printLn();
        
    }
    
    return;
}

void registerCommandTASK(void) {
    
    uint8_t taskCommandName[] = "task";
    
    ConsoleRegisterCommand(taskCommandName, sizeof(taskCommandName), functionTASK);
    
    return;
}
