#ifndef __DISPLAY_DRIVER_
#define __DISPLAY_DRIVER_

#include <avr/io.h>

#include <kernel/bus.h>
#include <kernel/device.h>

extern struct DisplayDeviceDriver* displayDriver;


struct DisplayDeviceDriver {
    
    struct Device device;
    
    struct Bus interface;
    
    
    void(*read)(uint32_t address, char* buffer);
    
    void(*write)(uint32_t address, char buffer);
    
    // Custom functions here
    
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



void initiateDisplayDriver(void);


void __read_display_device(uint32_t address, char* buffer);

void __write_display_device(uint32_t address, char buffer);


void __print(uint8_t line, uint8_t position, char* string, unsigned int length);

void __print_mask(uint8_t line, uint8_t position, char* string, unsigned int length);


void __set_cursor_line(uint8_t line);

void __set_cursor_position(uint8_t position);

void __set_cursor_prompt(uint8_t prompt);

void __set_cursor_blink_rate(uint8_t rate);

void __set_refresh_rate(uint8_t rate);


void __clear_frame_buffer(void);

void __clear_mask_buffer(void);

void __clear_line(uint8_t line);


void __shift_frame_up(void);

void __shift_frame_down(void);

#endif
