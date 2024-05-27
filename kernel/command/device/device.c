#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/device/device.h>

void functionDevice(uint8_t* param, uint8_t param_length) {
    
    uint8_t numberOfDevices = GetNumberOfDevices();
    
    for (uint8_t i=0; i < numberOfDevices; i++) {
        
        struct Device* dev = GetHardwareDeviceByIndex( i );
        
        print(dev->device_name, 10);
        printSpace(1);
        
        uint8_t deviceAddr[] = "0x00000";
        
        uint8_t addr = (dev->hardware_address) / 0x10000;
        
        deviceAddr[2] += addr;
        print(&deviceAddr[0], sizeof(deviceAddr));
        
        printLn();
        
        /*
        
        //
        // This will list the drivers rather than devices
        //
        
        struct Driver* drv = GetDriverByIndex( i );
        
        print(drv->device.device_name, 10);
        printSpace(1);
        
        
        uint8_t deviceAddr[] = "0x00000";
        
        uint8_t addr = (drv->device.hardware_address - 0x40000) / 0x10000;
        deviceAddr[2] += addr;
        print(&deviceAddr[0], sizeof(deviceAddr));
        
        printLn();
        
        */
        
    }
    
    return;
}



void registerCommandDevice(void) {
    
    uint8_t deviceCommandName[] = "device";
    
    ConsoleRegisterCommand(deviceCommandName, sizeof(deviceCommandName), functionDevice);
    
    return;
}
