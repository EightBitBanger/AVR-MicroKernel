extern "C" {
#include <kernel/kernel.h>
#include <kernel/command/test/test.h>
}

#include <kernel/array.hpp>
#include <kernel/string.hpp>


void functionTest(uint8_t* param, uint8_t param_length) {
    
    for (uint8_t i=0; i < 10; i++) {
        
        array arr(100);
        
        array test( arr );
        
    }
    
    
    
    return;
}

void registerCommandTest(void) {
    
    uint8_t testCommandName[] = "test";
    
    ConsoleRegisterCommand(testCommandName, sizeof(testCommandName), functionTest);
    
    return;
}
