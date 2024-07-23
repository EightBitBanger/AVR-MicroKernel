#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/driver/driver.h>

// Hot-loaded drivers
uint8_t number_of_loaded_drivers = 0;

struct Driver loaded_drivers[NUMBER_OF_DRIVERS];

// Registry of drivers
void* driver_registry[ DRIVER_REGISTRY_SIZE ];

// Number of drivers registered in the kernel
uint8_t number_of_drivers = 0;

/*
  
  Driver file layout
    
    Driver file ID         2 byte
    
    Driver name           10 Bytes
    
    Marker byte            1 byte '$'
    
    Device ID              1 byte
    
    Bus read WS            1 byte
    Bus write WS           1 byte
    
    Bus interface type     1 byte
      Type 0 - Default bus
      Type 1 - Memory bus
      Type 2 - IO bus
    
    
*/

int8_t LoadLibrary(uint8_t* filename, uint8_t filenameLength) {
    
    uint32_t fileAddress = fsFileExists(filename, filenameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    fsFileOpen(fileAddress);
    
    uint32_t fileSize = fsFileGetSize();
    
    uint8_t fileBuffer[fileSize];
    
    for (uint8_t i=0; i < fileSize; i++) 
        fileBuffer[i] = ' ';
    
    fsFileRead(fileBuffer, fileSize);
    
    fsFileClose();
    
    // Check for an available driver slot
    for (uint8_t i=0; i < NUMBER_OF_DRIVERS; i++) {
        
        if (loaded_drivers[i].device.device_id != 0) 
            continue;
        
        // Check driver header bytes
        if ((fileBuffer[0] != 'K') & 
            (fileBuffer[1] != 'D')) 
            return -1;
        
        // Check marker byte
        if (fileBuffer[DEVICE_NAME_LENGTH + 2] != '$') 
            return -1;
        
        // Load the driver
        
        // Name
        for (uint8_t a=0; a < DEVICE_NAME_LENGTH; a++) 
            loaded_drivers[i].device.device_name[a] = fileBuffer[a + 2];
        
        loaded_drivers[i].device.device_id = fileBuffer[DEVICE_NAME_LENGTH + 3];
        
        // Will be set upon hardware recognition
        loaded_drivers[i].device.hardware_slot = 0;
        loaded_drivers[i].device.hardware_address = 0;
        
        // Set the bus interface type
        loaded_drivers[i].interface.read_waitstate  = fileBuffer[DEVICE_NAME_LENGTH + 4];
        loaded_drivers[i].interface.write_waitstate = fileBuffer[DEVICE_NAME_LENGTH + 5];
        loaded_drivers[i].interface.bus_type = fileBuffer[DEVICE_NAME_LENGTH + 6];
        
        switch (loaded_drivers[i].interface.bus_type) {
        
        default:
            
        case 0:
            loaded_drivers[i].read  = (void(*)(uint32_t, uint8_t*)) bus_read_byte;
            loaded_drivers[i].write = (void(*)(uint32_t, uint8_t))  bus_write_byte;
            break;
            
        case 1:
            loaded_drivers[i].read  = (void(*)(uint32_t, uint8_t*)) bus_read_memory;
            loaded_drivers[i].write = (void(*)(uint32_t, uint8_t))  bus_write_memory;
            break;
            
        case 2:
            loaded_drivers[i].read  = (void(*)(uint32_t, uint8_t*)) bus_read_io;
            loaded_drivers[i].write = (void(*)(uint32_t, uint8_t))  bus_write_io;
            break;
            
        }
        
        loaded_drivers[i].is_linked = 1;
        
        RegisterDriver( (void*)&loaded_drivers[i] );
        
        return 1;
    }
    
    return 0;
}


struct Driver* GetDriverByName(uint8_t* name, uint8_t nameLength) {
	
	for (uint8_t d=0; d < number_of_drivers; d++) {
        
        struct Driver* driverPtr = GetDriverByIndex(d);
        
        uint8_t isWrongName = 0;
        
        for (uint8_t i=0; i < nameLength - 1; i++) {
            
            if (driverPtr->device.device_name[i] != name[i]) {
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

struct Driver* GetDriverByIndex(uint8_t index) {
    
    return driver_registry[ index ];
}


uint8_t RegisterDriver(void* deviceDriverPtr) {
    
    uint8_t current_number_of_drivers = number_of_drivers;
    
    driver_registry[ number_of_drivers ] = (void*)deviceDriverPtr;
    
    number_of_drivers++;
    
    return current_number_of_drivers;
}

uint8_t GetNumberOfDrivers(void) {
    
    return number_of_drivers;
}

void driverInit(void) {
    
    for (uint8_t i=0; i < NUMBER_OF_DRIVERS; i++) {
        
        for (uint8_t a=0; a < DEVICE_NAME_LENGTH; a++) 
            loaded_drivers[i].device.device_name[a] = ' ';
        
        loaded_drivers[i].device.device_id = 0;
        loaded_drivers[i].device.hardware_slot = 0;
        loaded_drivers[i].device.hardware_address = 0;
        
        loaded_drivers[i].interface.read_waitstate = 0;
        loaded_drivers[i].interface.write_waitstate = 0;
        loaded_drivers[i].interface.bus_type = 0;
        
        loaded_drivers[i].is_linked = 0;
        
        loaded_drivers[i].read  = (void(*)(uint32_t, uint8_t*))0;
        loaded_drivers[i].write = (void(*)(uint32_t, uint8_t))0;
        
    }
    
};
