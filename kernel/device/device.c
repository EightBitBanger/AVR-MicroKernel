#include <kernel/device/device.h>

struct Device device_table[ DEVICE_TABLE_SIZE ];


void initiateDeviceTable(void) {
    
    // Clear the device table
    for (unsigned int d=0; d < DEVICE_TABLE_SIZE; d++) {
        
        device_table[d].hardware_address = 0x00000;
        
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH; i++) 
            device_table[d].device_name[i] = ' ';
    }
    
    struct Bus bus;
    
    bus.read_waitstate  = 20;
    bus.write_waitstate = 20;
    
    // Check peripheral devices
    
    unsigned int index=0;
    
    for (unsigned int d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        // Set the device address
        uint32_t hardware_address = PERIPHERAL_ADDRESS_BEGIN + (PERIPHERAL_STRIDE * d);
        
        char nameBuffer[ DEVICE_NAME_LENGTH ];
        
        // Get device name
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH; i++) 
            bus_read_byte(&bus, hardware_address + i, &nameBuffer[i]);
        
        // Reject device name
        if (is_letter( &nameBuffer[1] ) == 0) 
            continue;
        
        // Add device to the device table
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH; i++) 
            device_table[ index ].device_name[i] = nameBuffer[i];
        
        device_table[d].hardware_address = hardware_address;
        
        index++;
        
        continue;
    }
    
    return;
}


struct Device* GetDevice(unsigned int deviceIndex) {
    return &device_table[ deviceIndex ];
}


