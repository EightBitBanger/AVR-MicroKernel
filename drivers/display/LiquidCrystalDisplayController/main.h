#ifndef __DISPLAY_DRIVER_
#define __DISPLAY_DRIVER_

#include <avr/io.h>

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>

struct DisplayDeviceDriver {
    
    struct Device device;
    
    struct Bus interface;
    
    uint8_t is_linked;
    
    void(*read)(uint32_t address, uint8_t* buffer);
    void(*write)(uint32_t address, uint8_t buffer);
    
};

void initiateDisplayDriver(void);

#endif
