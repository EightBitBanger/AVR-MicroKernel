extern "C" {
#include <kernel/kernel.h>
#include <kernel/command/test/test.h>
}

#include <kernel/array.hpp>
#include <kernel/string.hpp>

void _tsr_callback(uint8_t messages);


uint8_t applicataionBuffer[] = {
    0x89, 0x01, 0x09, 0x83, 0x06, 0x15, 0x00, 0x00, 0x00, 0xCD, 0x10, 0x9A, 0x22, 0x00, 0x00, 0x00, 0xFE, 0x33, 0x00, 0x00, 0x00, 0x62, 0x65, 0x67, 
    0x69, 0x6E, 0x63, 0x61, 0x6C, 0x6C, 0x63, 0x61, 0x6C, 0x6C, 0x89, 0x01, 0x09, 0x83, 0x06, 0x1A, 0x00, 0x00, 0x00, 0xCD, 0x10, 0xFE, 0x33, 0x00, 
    0x00, 0x00, 0xCB, 0x89, 0x01, 0x09, 0x83, 0x06, 0x1E, 0x00, 0x00, 0x00, 0xCD, 0x10, 0xCD, 0x20, 0xCB, 
};



void functionTest(uint8_t* param, uint8_t param_length) {
    
    uint32_t fileSizeTotal = sizeof(applicataionBuffer);
    
    uint8_t filename[] = "run";
    
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), fileSizeTotal);
    
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    int32_t index = fsFileOpen(fileAddress);
    
    fsFileWrite(index, applicataionBuffer, fileSizeTotal);
    fsFileClose(index);
    
    return;
    
    /*
    if (TaskCreate(param, param_length, _tsr_callback, TASK_PRIORITY_REALTIME, TASK_PRIVILEGE_USER, TASK_TYPE_TSR) == -1) {
        uint8_t msgFailed[] = "Failed";
        print(msgFailed, sizeof(msgFailed));
        return;
    }
    */
    
    return;
}


void _tsr_callback(uint8_t messages) {
    
    _delay_ms(30);
    
    switch (messages) {
        
        case EVENT_INITIATE: {
            break;
        }
        
        case EVENT_SHUTDOWN: {
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
