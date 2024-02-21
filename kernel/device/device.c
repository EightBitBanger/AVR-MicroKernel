#include <kernel/device/device.h>

void* device_registry[DEVICE_REGISTRY_SIZE];

struct Device device_table[ DEVICE_TABLE_SIZE ];


uint8_t number_of_devices = 0;



void InitiateDeviceTable(void) {
    
    // Clear the device table
    for (unsigned int d=0; d < DEVICE_TABLE_SIZE; d++) {
        
        device_table[d].hardware_address = 0x00000;
        
        device_table[d].device_id = 0x00;
        
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
        
        device_table[d].device_id = nameBuffer[0];
        
        index++;
        
        continue;
    }
    
    // Link the hardware to the device drivers
    
    for (unsigned int i=0; i < number_of_devices; i++) {
        
        for (unsigned int d=0; d < DEVICE_TABLE_SIZE; d++) {
            
            struct Device* registryEntry = (struct Device*)device_registry[i];
            
            if (registryEntry->device_id != device_table[d].device_id) 
                continue;
            
            registryEntry->hardware_address = device_table[d].hardware_address;
            
            i = number_of_devices;
            break;
        }
        
    }
    
    
    
    return;
}


struct Device* GetHardwareDevice(unsigned int deviceIndex) {
    return &device_table[ deviceIndex ];
}



uint8_t RegisterDriver(void* deviceDriverPtr) {
    
    uint8_t current_number_of_devices = number_of_devices;
    
    device_registry[ number_of_devices ] = (void*)deviceDriverPtr;
    
    number_of_devices++;
    
    return current_number_of_devices;
}

