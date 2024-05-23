#include <avr/io.h>

#include <kernel/delay.h>

#include <kernel/device/device.h>
#include <kernel/driver/driver.h>


struct Device device_table[ DEVICE_TABLE_SIZE ];

// Number of devices on the system bus
uint8_t number_of_devices = 0;


void InitiateDeviceTable(void) {
    
    // Clear the device table
    for (unsigned int d=0; d < DEVICE_TABLE_SIZE; d++) {
        
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH; i++) 
            device_table[d].device_name[i] = ' ';
        
        device_table[d].hardware_slot = 0;
        
        device_table[d].device_id = 0x00;
        
        device_table[d].hardware_address = 0x00000;
        
    }
    
    struct Bus bus;
    
    bus.read_waitstate  = 20;
    bus.write_waitstate = 20;
    
    // Check peripheral devices
    
    unsigned int index=0;
    
    for (uint32_t d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        // Set the device address
        uint32_t hardware_address = PERIPHERAL_ADDRESS_BEGIN + (PERIPHERAL_STRIDE * d);
        
        // Get device header
        uint8_t buffer[ 10 ];
        
        for (unsigned int i=0; i < 10; i++) 
            bus_read_byte(&bus, hardware_address + i, &buffer[i]);
        
        // Reject device name
        if (is_letter( &buffer[1] ) == 0) 
            continue;
        
        if (buffer[1] == ' ') 
            continue;
        
        // Add device to the device table
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH - 1; i++) 
            device_table[ index ].device_name[i] = buffer[i + 1];
        
        device_table[index].hardware_address = hardware_address;
        
        device_table[index].hardware_slot = d + '1';
        
        device_table[index].device_id = buffer[0];
        
        number_of_devices++;
        
        index++;
        
        continue;
    }
    
    // Link the hardware to the device drivers
    
    for (unsigned int i=0; i < GetNumberOfDrivers(); i++) {
        
        for (unsigned int d=0; d < DEVICE_TABLE_SIZE; d++) {
            
            struct Device* registryEntry = (struct Device*)GetDriverByIndex(i);
            
            if (registryEntry->device_id != device_table[d].device_id) 
                continue;
            
            registryEntry->hardware_address = device_table[d].hardware_address;
            
            break;
        }
        
    }
    
    return;
}




struct Device* GetHardwareDeviceByIndex(uint8_t index) {
    return &device_table[ index ];
}


uint8_t GetNumberOfDevices(void) {
    return number_of_devices;
}
