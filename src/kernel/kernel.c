#include <avr/io.h>

#include <kernel/kernel.h>

#define PERIPHERAL_ADDRESS_BEGIN  0x40000

#define NUMBER_OF_PERIPHERALS  5


struct Device device_table[32];


void kernel_initiate(void) {
    
    struct Bus bus;
    
    bus.read_waitstate  = 20;
    bus.write_waitstate = 20;
    
    // Check peripheral slots
    for (unsigned int d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        device_table[d].hardware_address = 0;
        
        char nameChar=0x00;
        
        // Copy device name
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH; i++) {
            
            bus_read_byte(&bus, PERIPHERAL_ADDRESS_BEGIN + i, &nameChar);
            
            device_table[d].device_name[i] = nameChar;
        }
        
        continue;
    }
    
    return;
}


struct Device* get_kernel_get_device(unsigned int deviceIndex) {
    
    return 0;
}


