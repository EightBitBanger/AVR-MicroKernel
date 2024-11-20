extern "C" {
#include <kernel/kernel.h>
#include <kernel/command/test/test.h>
}

#include <kernel/array.hpp>
#include <kernel/string.hpp>

uint32_t address = nullptr;



void _tsr_callback(uint8_t messages) {
    
    switch (messages) {
        
        case EVENT_INITIATE: {
            
            break;
        }
        
        case EVENT_SHUTDOWN: {
            
            break;
        }
        
    }
    
    if (address == nullptr) 
        return;
    
    fsFree(0);
    
    return;
}









void functionTest(uint8_t* param, uint8_t param_length) {
    
    if (address != nullptr) 
        return;
    
    uint8_t taskName[] = "kernel";
    TaskCreate(taskName, sizeof(taskName), _tsr_callback, TASK_PRIORITY_REALTIME, TASK_PRIVILEGE_USER, TASK_TYPE_TSR);
    
    return;
}



void registerCommandTest(void) {
    
    uint8_t testCommandName[] = "test";
    
    ConsoleRegisterCommand(testCommandName, sizeof(testCommandName), functionTest);
    
    return;
}
