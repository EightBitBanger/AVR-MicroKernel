#ifndef __DISPLAY_DRIVER_
#define __DISPLAY_DRIVER_

#include <avr/io.h>

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>

struct DisplayDeviceDriver {
    
    struct Device device;
    
    struct Bus interface;
    
    // Driver functions
    
    void(*read)(uint32_t address, char* buffer);
    void(*write)(uint32_t address, char buffer);
    
    //
    // Custom functions
    
    void(*print)(uint8_t line, uint8_t position, char* string, unsigned int length);
    void(*printMask)(uint8_t line, uint8_t position, char* string, unsigned int length);
    
    void(*cursorSetLine)(uint8_t line);
    void(*cursorSetPosition)(uint8_t position);
    void(*cursorSetPrompt)(uint8_t prompt);
    void(*cursorSetBlinkRate)(uint8_t rate);
    void(*setRefreshRate)(uint8_t rate);
    
    void(*clearFrameBuffer)(void);
    void(*clearMaskBuffer)(void);
    void(*clearLine)(uint8_t line);
    
    void(*shiftFrameUp)(void);
    void(*shiftFrameDown)(void);
    
};

extern struct DisplayDeviceDriver* displayDriver;

void initiateDisplayDriver(void);

#endif
