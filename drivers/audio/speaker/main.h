#ifndef __SPEAKER_BUZZER_DRIVER_
#define __SPEAKER_BUZZER_DRIVER_

#include <avr/io.h>

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>

void spkBeep(uint16_t duration, uint16_t frequency);

struct SpeakerDeviceDriver {
    
    struct Device device;
    
    struct Bus interface;
    
    void(*read)(uint32_t address, uint8_t* buffer);
    void(*write)(uint32_t address, uint8_t buffer);
    
};

void initiateSpeakerDriver(void);

#endif
