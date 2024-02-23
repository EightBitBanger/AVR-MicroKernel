#include <kernel/device/device.h>

void* driver_registry[DEVICE_REGISTRY_SIZE];

struct Device device_table[ DEVICE_TABLE_SIZE ];

// Number of drivers registered in the kernel
uint8_t number_of_drivers = 0;

// Number of devices on the system bus
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
        
        number_of_devices++;
        
        index++;
        
        continue;
    }
    
    // Link the hardware to the device drivers
    
    for (unsigned int i=0; i < number_of_drivers; i++) {
        
        for (unsigned int d=0; d < DEVICE_TABLE_SIZE; d++) {
            
            struct Device* registryEntry = (struct Device*)driver_registry[i];
            
            if (registryEntry->device_id != device_table[d].device_id) 
                continue;
            
            registryEntry->hardware_address = device_table[d].hardware_address;
            
            i = number_of_drivers;
            break;
        }
        
    }
    
    
    
    return;
}




struct Driver* GetDriverByName(char* nameString, uint8_t stringSize) {
	
	for (uint8_t d=0; d < number_of_drivers; d++) {
        
        struct Driver* driverPtr = GetDriverByIndex(d);
        
        uint8_t isWrongName = 0;
        
        for (uint8_t i=0; i < stringSize - 1; i++) {
            
            if (driverPtr->device.device_name[i] != nameString[i]) {
                isWrongName = 1;
                break;
            }
            
        }
        
        if (isWrongName == 0) 
            return driverPtr;
        
        continue;
	}
	
	return nullptr;
}


struct Device* GetHardwareDeviceByIndex(uint8_t index) {
    return &device_table[ index ];
}

struct Driver* GetDriverByIndex(uint8_t index) {
    return driver_registry[ index ];
}


uint8_t RegisterDriver(void* deviceDriverPtr) {
    
    uint8_t current_number_of_drivers = number_of_drivers;
    
    driver_registry[ number_of_drivers ] = (void*)deviceDriverPtr;
    
    number_of_drivers++;
    
    return current_number_of_drivers;
}


uint8_t GetNumberOfDevices(void) {
    return number_of_devices;
}

uint8_t GetNumberOfDrivers(void) {
    return number_of_drivers;
}
