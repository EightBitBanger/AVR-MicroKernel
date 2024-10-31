extern "C" {
#include <kernel/kernel.h>
#include <kernel/command/test/test.h>
}

#include <kernel/array.hpp>
#include <kernel/string.hpp>

uint32_t address = 0;

void test(uint8_t messages) {
    
    switch (messages) {
        
        case EVENT_INITIATE: {
            
            if (address == nullptr) 
                address = new(40);
            
            break;
        }
        
        case EVENT_SHUTDOWN: {
            
            if (address != nullptr) 
                delete(address);
            
            address = nullptr;
            
            break;
        }
        
    }
    
    if (address == nullptr) 
        return;
    
    return;
}



void functionTest(uint8_t* param, uint8_t param_length) {
    
    if (address != nullptr) 
        return;
    
    uint8_t taskName[] = "test";
    
    TaskCreate(taskName, sizeof(taskName), test, TASK_PRIORITY_HIGH, TASK_TYPE_USER);
    
    return;
}

void registerCommandTest(void) {
    
    uint8_t testCommandName[] = "test";
    
    ConsoleRegisterCommand(testCommandName, sizeof(testCommandName), functionTest);
    
    return;
}
