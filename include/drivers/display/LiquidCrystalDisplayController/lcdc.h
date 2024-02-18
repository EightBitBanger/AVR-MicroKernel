#ifndef __DISPLAY_DRIVER_
#define __DISPLAY_DRIVER_

#include <kernel/bus.h>
#include <kernel/device.h>


void initiateDisplayDriver(void);

void __read_display_device(uint32_t address, char* buffer);
void __write_display_device(uint32_t address, char buffer);


struct DisplayDeviceDriver {
    
    struct Device device;
    
    struct Bus interface;
    
    
    void(*read)(uint32_t address, char* buffer);
    void(*write)(uint32_t address, char buffer);
    
    
} displayDriver;




//
// Implementations
//

void initiateDisplayDriver(void) {
    
    displayDriver.read = __read_display_device;
    displayDriver.write = __write_display_device;
    
    return;
}

void __read_display_device(uint32_t address, char* buffer) {
    
    bus_read_byte( &displayDriver.interface, displayDriver.device.hardware_address + address, buffer );
    
    return;
}

void __write_display_device(uint32_t address, char buffer) {
    
    bus_write_byte( &displayDriver.interface, displayDriver.device.hardware_address + address, buffer );
    
    return;
}


















#endif
