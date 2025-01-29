extern "C" {
#include <kernel/kernel.h>
#include <kernel/command/test/test.h>
}

#include <kernel/array.hpp>
#include <kernel/string.hpp>

void _tsr_callback(uint8_t messages);


void functionTest(uint8_t* param, uint8_t param_length) {
    
    /*
    array arr(20);
    
    for (uint16_t i=0; i < arr.size(); i++) 
        arr.set(i, i);
    
    for (uint16_t i=0; i < arr.size(); i++) {
        
        printInt( arr[i] );
        
        printLn();
        
    }
    */
    
    uint8_t taskName[] = "test";
    TaskCreate(taskName, sizeof(taskName), _tsr_callback, TASK_PRIORITY_REALTIME, TASK_PRIVILEGE_USER, TASK_TYPE_TSR);
    
    return;
}


uint32_t superBlock;
uint16_t counter = 0;

void _tsr_callback(uint8_t messages) {
    
    switch (messages) {
        
        case EVENT_INITIATE: {
            
            superBlock = new(64);
            
            break;
        }
        
        case EVENT_SHUTDOWN: {
            
            delete(superBlock);
            
            break;
        }
        
    }
    
    return;
}


void registerCommandTest(void) {
    
    uint8_t testCommandName[] = "test";
    
    ConsoleRegisterCommand(testCommandName, sizeof(testCommandName), functionTest);
    
    return;
}
