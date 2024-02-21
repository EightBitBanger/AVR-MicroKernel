#include <drivers/display/LiquidCrystalDisplayController/main.h>


struct DisplayDeviceDriver displayDeviceDriver;

struct DisplayDeviceDriver* displayDriver = &displayDeviceDriver;


// Driver function declarations

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


void initiateDisplayDriver(void) {
    
    char deviceName[] = "display";
    
    for (unsigned int i=0; i < sizeof(deviceName); i++) {
        displayDriver->device.device_name[i] = deviceName[i];
    }
    
	displayDriver->device.hardware_address = 0x00000;
	
	displayDriver->device.device_id = 0x10;
	
	displayDriver->interface.read_waitstate  = 10;
	displayDriver->interface.write_waitstate = 10;
    
	// Initiate member functions
	
    displayDriver->read                = __read_display_device;
    displayDriver->write               = __write_display_device;
    
    displayDriver->print               = __print;
    displayDriver->printMask           = __print_mask;
    
    displayDriver->cursorSetLine       = __set_cursor_line;
    displayDriver->cursorSetPosition   = __set_cursor_position;
    displayDriver->cursorSetPrompt     = __set_cursor_prompt;
    displayDriver->cursorSetBlinkRate  = __set_cursor_blink_rate;
    displayDriver->setRefreshRate      = __set_refresh_rate;
    
    displayDriver->clearFrameBuffer    = __clear_frame_buffer;
    displayDriver->clearMaskBuffer     = __clear_mask_buffer;
    displayDriver->clearLine           = __clear_line;
    
    displayDriver->shiftFrameUp        = __shift_frame_up;
    displayDriver->shiftFrameDown      = __shift_frame_down;
    
    return;
}


void __read_display_device(uint32_t address, char* buffer) {
    bus_read_byte( &displayDriver->interface, displayDriver->device.hardware_address + address, buffer );
    return;
}

void __write_display_device(uint32_t address, char buffer) {
    bus_write_byte( &displayDriver->interface, displayDriver->device.hardware_address + address, buffer );
    return;
}

void __print(uint8_t line, uint8_t position, char* string, unsigned int length) {
    for (unsigned int i=0; i < length - 1; i++) 
        displayDriver->write( (line * 20) + position + i, string[i] );
    return;
}

void __print_mask(uint8_t line, uint8_t position, char* string, unsigned int length) {
    for (unsigned int i=0; i < length - 1; i++) 
        displayDriver->write( (line * 20) + position + i + 80, string[i] );
    return;
}

void __set_cursor_line(uint8_t line) {
    displayDriver->write( 0xa0, line );
    return;
}

void __set_cursor_position(uint8_t position) {
    displayDriver->write( 0xa1, position );
    return;
}

void __set_cursor_prompt(uint8_t prompt) {
    displayDriver->write( 0xa2, prompt );
    return;
}

void __set_cursor_blink_rate(uint8_t rate) {
    displayDriver->write( 0xa3, rate );
    return;
}

void __set_refresh_rate(uint8_t rate) {
    displayDriver->write( 0xa4, rate );
    return;
}

void __clear_frame_buffer(void) {
    displayDriver->write( 0xa5, 0x01 );
    return;
}

void __clear_mask_buffer(void) {
    displayDriver->write( 0xa6, 0x01 );
    return;
}

void __clear_line(uint8_t line) {
    displayDriver->write( 0xa7, line );
    return;
}

void __shift_frame_up(void) {
    displayDriver->write( 0xa8, 0x01 );
    return;
}

void __shift_frame_down(void) {
    displayDriver->write( 0xa9, 0x01 );
    return;
}
