<<<<<<< HEAD
extern "C" {
#include <kernel/kernel.h>
#include <kernel/command/test/test.h>
}

void functionTest(uint8_t* param, uint8_t param_length) {
    
    uint8_t driverName[] = "network";
    Driver* networkDriver = GetDriverByName(driverName, sizeof(driverName)-1);
    
    if (networkDriver == nullptr) {
        uint8_t driverError[] = "Driver not found";
        print(driverError, sizeof(driverError));
        printLn();
        return;
    }
    
    uint8_t buffer[30];
    for (unsigned int i=0; i < 30; i++) 
        bus_read_byte(&networkDriver->device.bus, networkDriver->device.hardware_address + i, &buffer[i]);
    
    print(buffer, 30);
    printLn();
    
    return;
}


void registerCommandTest(void) {
    
    uint8_t testCommandName[] = "test";
    
    ConsoleRegisterCommand(testCommandName, sizeof(testCommandName), functionTest);
    
    return;
}
=======
extern "C" {
#include <kernel/kernel.h>
#include <kernel/command/test/test.h>
}

extern "C" struct Driver* displayDevice;


/*
int8_t vertexBuffer[] = {
    -10, -10, -10,   10, -10, -10,   10,  10, -10,  2, // Front face
     10,  10, -10,  -10,  10, -10,  -10, -10, -10,  2, 
     10, -10, -10,   10, -10,  10,   10,  10,  10,  4, // Right face
     10,  10,  10,   10,  10, -10,   10, -10, -10,  4, 
     10, -10,  10,  -10, -10,  10,  -10,  10,  10,  3, // Back face
    -10,  10,  10,   10,  10,  10,   10, -10,  10,  3, 
    -10, -10,  10,  -10, -10, -10,  -10,  10, -10,  2, // Left face
    -10,  10, -10,  -10,  10,  10,  -10, -10,  10,  2, 
    -10,  10, -10,   10,  10, -10,   10,  10,  10,  4, // Top face
     10,  10,  10,  -10,  10,  10,  -10,  10, -10,  4, 
    -10, -10, -10,  -10, -10,  10,   10, -10,  10,  3, // Bottom face
     10, -10,  10,   10, -10, -10,  -10, -10, -10,  3 , 
};
*/

void functionTest(uint8_t* param, uint8_t param_length) {
    
    
    
    return;
    
    glInit(GL_MODE_SPRITES);
    glClear(0);
    
    __glBusyWait();
    displayDevice->write(0x00001, 0);
    
    __glBusyWait();
    displayDevice->write(0x00002, 0);
    
    // Write sprite
    /*
    __glBusyWait();
    displayDevice->write(0x00007, 2);
    for (unsigned int i=0; i < 8; i++) {
        __glBusyWait();
        displayDevice->write(0x00010 + i, 0xff);
    }
    */
    
    unsigned int numberOfChars = 0x0f;
    if (param[0] == 'f') 
        numberOfChars = 0x10;
    
    // Draw sprites
    __glBusyWait();
    displayDevice->write(0x00007, 1);
    for (unsigned int i=0; i < numberOfChars; i++) {
        __glBusyWait();
        displayDevice->write(0x00010 + i, 1);
    }
    
    swapBuffers();
    
    
    //glInit(GL_MODE_TEXT);
    
    return;
}


void registerCommandTest(void) {
    
    uint8_t testCommandName[] = "test";
    
    ConsoleRegisterCommand(testCommandName, sizeof(testCommandName), functionTest);
    
    return;
}
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
