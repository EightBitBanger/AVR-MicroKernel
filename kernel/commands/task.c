#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/task.h>


void functionTASK(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgTaskFound[]     = "Task stopped";
    uint8_t msgTaskNotFound[]  = "Task not found";
    
    lowercase(&param[0]);
    
    // List tasks
    if ((param[0] == ' ') & (param[1] == ' ')) {
        
        struct ProcessDescription proc_desc;
        
        for (uint8_t i=0; i < TASK_LIST_SIZE; i++) {
            
            if (GetProcInfo(i, &proc_desc) == 0) 
                continue;
            
            // Type
            printChar( proc_desc.type );
            printSpace(1);
            // Name
            print(proc_desc.name, TASK_NAME_LENGTH_MAX);
            printSpace(2);
            // Priority
            if (proc_desc.priority == TASK_PRIORITY_NORMAL) {
                
                printc("normal", 7);
                
            } else {
                
                if (proc_desc.priority == TASK_PRIORITY_HALT) {
                    
                    printc("halted", 7);
                    
                } else {
                    
                    if (proc_desc.priority < TASK_PRIORITY_NORMAL) printc("high", 5);
                    if (proc_desc.priority > TASK_PRIORITY_NORMAL) printc("low", 4);
                    
                }
            }
            
            printLn();
            
        }
        
        return;
    }
    
    // Kill a task
    if ((param[0] == 'k') & (param[1] == ' ')) {
        
        if (TaskFind(&param[2], param_length - 2) != 0) {
            
            print(msgTaskFound, sizeof(msgTaskFound));
            printLn();
            
            TaskKill(&param[2], param_length - 2);
            
        } else {
            
            print(msgTaskNotFound, sizeof(msgTaskNotFound));
            printLn();
        }
        
        return;
    }
    
    return;
}

void registerCommandTASK(void) {
    
    uint8_t taskCommandName[] = "task";
    
    ConsoleRegisterCommand(taskCommandName, sizeof(taskCommandName), functionTASK);
    
    return;
}
