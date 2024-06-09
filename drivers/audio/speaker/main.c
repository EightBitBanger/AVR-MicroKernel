#include <drivers/audio/speaker/main.h>

#include <kernel/kernel.h>

struct SpeakerDeviceDriver speakerDeviceDriver;

struct SpeakerDeviceDriver* speakerDriver = &speakerDeviceDriver;


// Driver function declarations

void __read_speaker_device(uint32_t address, uint8_t* buffer);
void __write_speaker_device(uint32_t address, uint8_t buffer);


void initiateSpeakerDriver(void) {
    
    uint8_t deviceName[] = "speaker";
    
    for (unsigned int i=0; i < sizeof(deviceName); i++) {
        speakerDriver->device.device_name[i] = deviceName[i];
    }
    
    // Register the driver with the kernel
	RegisterDriver( (void*)speakerDriver );
	
	// Set hardware device details
	speakerDriver->device.hardware_address = 0x60000;
	
	speakerDriver->device.device_id = 0x60;
	
	speakerDriver->is_linked = 0;
	
	speakerDriver->interface.read_waitstate  = 1000;
	speakerDriver->interface.write_waitstate = 1000;
    
    
    speakerDriver->read  = __read_speaker_device;
    speakerDriver->write = __write_speaker_device;
    
    return;
}


void __read_speaker_device(uint32_t address, uint8_t* buffer) {
    bus_read_byte( &speakerDriver->interface, speakerDriver->device.hardware_address + address, buffer );
    return;
}

void __write_speaker_device(uint32_t address, uint8_t buffer) {
    bus_write_byte( &speakerDriver->interface, speakerDriver->device.hardware_address + address, buffer );
    return;
}


void spkBeep(uint16_t duration, uint16_t frequency) {
    
    for (uint16_t i=0; i < duration; i++) {
        
        speakerDriver->write(0, 0xff);
        
        for (uint16_t i=0; i < frequency / 16; i++) 
            __asm__("nop");
        
        continue;
    }
    
    return;
}

