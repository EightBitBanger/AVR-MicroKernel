#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/driver/driver.h>

/*
 
 Driver file layout
    
    Driver file ID         2 bytes 'KD'
    
    Driver name           10 Bytes
    
    Marker byte            1 byte '$'
    
    Device ID              1 byte
    
    Bus read waitstate     1 byte
    Bus write waitstate    1 byte
    
    Bus interface type     1 byte
      Type 0 - Default bus
      Type 1 - Memory bus
      Type 2 - IO bus
      Type 3 - Serial bus
    
    Hardware address       4 bytes
    
    
 Function blocks
    
    Number of addresses    4 bytes
    
    [address a][address b][address c]...
    
    [begin address a] ... [return]
    [begin address b] ... [return]
    [begin address c] ... [return]
    
*/

// List of all loaded drivers
struct Driver loaded_drivers[DRIVER_TABLE_SIZE];

// Registry of drivers actively running in the kernel
void* driver_registry[ DRIVER_REGISTRY_SIZE ];

// Number of drivers registered in the kernel
uint8_t number_of_drivers = 0;


int8_t LoadLibrary(uint8_t* filename, uint8_t filenameLength) {
    
    uint32_t fileAddress = fsFileExists(filename, filenameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    int32_t index = fsFileOpen(fileAddress);
    
    uint32_t fileSize = fsFileGetSize(fileAddress);
    
    uint8_t fileBuffer[fileSize];
    
    for (uint8_t i=0; i < fileSize; i++) 
        fileBuffer[i] = ' ';
    
    fsFileRead(index, fileBuffer, fileSize);
    
    fsFileClose(index);
    
    // Check driver header bytes
    if ((fileBuffer[0] != 'K') & (fileBuffer[1] != 'D')) 
        return -1;
    
    // Check marker byte
    if (fileBuffer[DEVICE_NAME_LENGTH + 2] != '$') 
        return -1;
    
    // Check driver already loaded
    for (uint8_t i=0; i < DRIVER_TABLE_SIZE; i++) {
        
        if (loaded_drivers[i].device.device_id == 0) 
            continue;
        
        // Check driver name
        if (StringCompare(loaded_drivers[i].device.device_name, DEVICE_NAME_LENGTH, &fileBuffer[2], DEVICE_NAME_LENGTH)) 
            return -2;
        
        continue;
    }
    
    // Check for an available driver slot
    for (uint8_t i=0; i < DRIVER_TABLE_SIZE; i++) {
        
        if (loaded_drivers[i].device.device_id != 0) 
            continue;
        
        // Name
        for (uint8_t a=0; a < DEVICE_NAME_LENGTH; a++) 
            loaded_drivers[i].device.device_name[a] = fileBuffer[a + 2];
        
        // Default non-linked driver
        loaded_drivers[i].is_linked = 0;
        loaded_drivers[i].device.hardware_slot = 0;
        
        // Set the bus interface type
        loaded_drivers[i].device.device_id            = fileBuffer[DEVICE_NAME_LENGTH + 3];
        loaded_drivers[i].device.bus.read_waitstate   = fileBuffer[DEVICE_NAME_LENGTH + 4];
        loaded_drivers[i].device.bus.write_waitstate  = fileBuffer[DEVICE_NAME_LENGTH + 5];
        loaded_drivers[i].device.bus.bus_type         = fileBuffer[DEVICE_NAME_LENGTH + 6];
        
        // Check bus type
        if ((loaded_drivers[i].device.bus.bus_type != 0) & 
            (loaded_drivers[i].device.bus.bus_type != 1) & 
            (loaded_drivers[i].device.bus.bus_type != 2))
            return -1;
        
        switch (loaded_drivers[i].device.bus.bus_type) {
            
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
        
        // Driver address pointer
        union Pointer addrPtr;
        addrPtr.byte_t[0] = fileBuffer[DEVICE_NAME_LENGTH + 7];
        addrPtr.byte_t[1] = fileBuffer[DEVICE_NAME_LENGTH + 8];
        addrPtr.byte_t[2] = fileBuffer[DEVICE_NAME_LENGTH + 9];
        addrPtr.byte_t[3] = fileBuffer[DEVICE_NAME_LENGTH + 10];
        
        loaded_drivers[i].device.hardware_address = addrPtr.address;
        
        
        // Link the driver to any associated
        // hardware devices on the bus
        uint8_t numberOfDevices = GetNumberOfDevices();
        
        for (uint8_t d=0; d < numberOfDevices; d++) {
            
            struct Device* devicePtr = GetDeviceByIndex(d);
            
            if (!StringCompare(devicePtr->device_name, DEVICE_NAME_LENGTH, &fileBuffer[2], DEVICE_NAME_LENGTH)) 
                continue;
            
            loaded_drivers[i].is_linked = 1;
            
            loaded_drivers[i].device.hardware_slot    = devicePtr->hardware_slot;
            loaded_drivers[i].device.hardware_address = devicePtr->hardware_address;
            
            if (loaded_drivers[i].is_linked == 1) 
                RegisterDriver( (void*)&loaded_drivers[i] );
            
            return 2;
        }
        
        // Add the driver to the registry only if
        // its linked to a hardware device
        if (loaded_drivers[i].is_linked == 1) 
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

void DriverTableInit(void) {
    
    for (uint8_t i=0; i < DRIVER_TABLE_SIZE; i++) {
        
        for (uint8_t a=0; a < DEVICE_NAME_LENGTH; a++) 
            loaded_drivers[i].device.device_name[a] = ' ';
        
        loaded_drivers[i].device.device_id = 0;
        loaded_drivers[i].device.hardware_slot = 0;
        loaded_drivers[i].device.hardware_address = 0;
        
        loaded_drivers[i].device.bus.read_waitstate = 0;
        loaded_drivers[i].device.bus.write_waitstate = 0;
        loaded_drivers[i].device.bus.bus_type = 0;
        
        loaded_drivers[i].is_linked = 0;
        
        loaded_drivers[i].read  = (void(*)(uint32_t, uint8_t*))0;
        loaded_drivers[i].write = (void(*)(uint32_t, uint8_t))0;
        
    }
    
};
