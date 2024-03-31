#include <avr/io.h>

#include <util/delay.h>

#include <kernel/device/device.h>
#include <kernel/driver/driver.h>


struct Device device_table[ DEVICE_TABLE_SIZE ];

// Number of devices on the system bus
uint8_t number_of_devices = 0;


void InitiateDeviceTable(void) {
    
    // Clear the device table
    for (unsigned int d=0; d < DEVICE_TABLE_SIZE; d++) {
        
        device_table[d].hardware_address = 0x00000;
        
        device_table[d].hardware_slot = 0;
        
        device_table[d].device_id = 0x00;
        
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH; i++) 
            device_table[d].device_name[i] = ' ';
    }
    
    struct Bus bus;
    
    bus.read_waitstate  = 40;
    bus.write_waitstate = 40;
    
    // Check peripheral devices
    
    unsigned int index=0;
    
    for (uint32_t d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        // Set the device address
        uint32_t hardware_address = PERIPHERAL_ADDRESS_BEGIN + (PERIPHERAL_STRIDE * d);
        
        // Get device header
        uint8_t nameBuffer[ 10 ];
        
        for (unsigned int i=0; i < 10; i++) 
            bus_read_byte(&bus, hardware_address + i, &nameBuffer[i]);
        
        // Reject device name
        if (is_letter( &nameBuffer[1] ) == 0) 
            continue;
        
        // Add device to the device table
        for (unsigned int i=0; i < DEVICE_NAME_LENGTH; i++) 
            device_table[ index ].device_name[i] = nameBuffer[i];
        
        device_table[index].hardware_address = hardware_address;
        
        device_table[index].hardware_slot = d + '1';
        
        device_table[index].device_id = nameBuffer[0];
        
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
    
    _delay_ms(80);
	
    return;
}




struct Device* GetHardwareDeviceByIndex(uint8_t index) {
    return &device_table[ index ];
}


uint8_t GetNumberOfDevices(void) {
    return number_of_devices;
}
