#ifndef __DISPLAY_DRIVER_
#define __DISPLAY_DRIVER_

#include <avr/io.h>

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>

enum DisplayFunctions {
    
    SET_CURSOR_LINE         = 0xa0,
    SET_CURSOR_POSITION     = 0xa1,
    SET_CURSOR_PROMPT       = 0xa2,
    SET_CURSOR_BLINK_RATE   = 0xa3,
    SET_REFRESH_RATE        = 0xa4,
    
    CLEAR_FRAME_BUFFER      = 0xa5,
    CLEAR_MASK_BUFFER       = 0xa6,
    CLEAR_LINE              = 0xa7,
    
    SHIFT_FRAME_UP          = 0xa8,
    SHIFT_FRAME_DOWN        = 0xa9
    
};

struct DisplayDeviceDriver {
    
    struct Device device;
    
    struct Bus interface;
    
    // Driver functions
    
    void(*read)(uint32_t address, uint8_t* buffer);
    void(*write)(uint32_t address, uint8_t buffer);
    
};

void initiateDisplayDriver(void);

#endif
