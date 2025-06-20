<<<<<<< HEAD
#ifndef __DISPLAY_DRIVER_
#define __DISPLAY_DRIVER_

#include <avr/io.h>

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>

struct DisplayDeviceDriver {
    
    struct Device device;
    
    uint8_t is_linked;
    
    void(*read)(uint32_t address, uint8_t* buffer);
    void(*write)(uint32_t address, uint8_t buffer);
    
};

void initiateDisplayDriver(void);

void __device_busy_wait(void);

#endif
=======
#ifndef __DISPLAY_DRIVER_
#define __DISPLAY_DRIVER_

#include <avr/io.h>

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>

struct DisplayDeviceDriver {
    
    struct Device device;
    
    uint8_t is_linked;
    
    void(*read)(uint32_t address, uint8_t* buffer);
    void(*write)(uint32_t address, uint8_t buffer);
    
};

void initiateDisplayDriver(void);

void __device_busy_wait(void);

#endif
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
