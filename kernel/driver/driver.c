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

struct Node* DriverTableHead = NULL;


int8_t LoadLibrary(uint8_t* filename, uint8_t filenameLength) {
    
    uint32_t fileAddress = fsFileExists(filename, filenameLength);
    
    if (fileAddress == 0) 
        return 0;
    
    int32_t index = fsFileOpen(fileAddress);
    uint32_t fileSize = fsFileGetSize(fileAddress);
    
    uint8_t fileBuffer[fileSize];
    fsFileRead(index, fileBuffer, fileSize);
    fsFileClose(index);
    
    // Check driver header bytes
    if ((fileBuffer[0] != 'K') & (fileBuffer[1] != 'D')) 
        return -1;
    
    // Check marker byte
    if (fileBuffer[DEVICE_NAME_LENGTH + 2] != '$') 
        return -1;
    
    // Check driver already loaded
    uint32_t numberOfDrivers = ListGetSize(DriverTableHead);
    
    for (uint8_t i=0; i < numberOfDrivers; i++) {
        
        struct Driver* driverPtr = (struct Driver*)ListGetNode(DriverTableHead, i)->data;
        
        if (driverPtr->device.device_id == 0) 
            continue;
        
        // Check driver name
        if (StringCompare(driverPtr->device.device_name, DEVICE_NAME_LENGTH, &fileBuffer[2], DEVICE_NAME_LENGTH)) 
            return -2;
        
        continue;
    }
    
    
    // Construct the driver and register it
    
    struct Driver* newDeviceDriver = (struct Driver*)malloc(sizeof(struct Driver));
    RegisterDriver( (void*)&newDeviceDriver );
    
    // Name
    for (uint8_t a=0; a < DEVICE_NAME_LENGTH; a++) 
        newDeviceDriver->device.device_name[a] = fileBuffer[a + 2];
    
    // Default non-linked blank driver
    newDeviceDriver->is_linked = 0;
    newDeviceDriver->device.hardware_slot = 0;
    
    // Set the bus interface type
    newDeviceDriver->device.device_id            = fileBuffer[DEVICE_NAME_LENGTH + 3];
    newDeviceDriver->device.bus.read_waitstate   = fileBuffer[DEVICE_NAME_LENGTH + 4];
    newDeviceDriver->device.bus.write_waitstate  = fileBuffer[DEVICE_NAME_LENGTH + 5];
    newDeviceDriver->device.bus.bus_type         = fileBuffer[DEVICE_NAME_LENGTH + 6];
    
    // Check correct bus types
    if ((newDeviceDriver->device.bus.bus_type != 0) & 
        (newDeviceDriver->device.bus.bus_type != 1) & 
        (newDeviceDriver->device.bus.bus_type != 2)) {
        
        return -1;
    }
    
    switch (newDeviceDriver->device.bus.bus_type) {
        
        default:
            
        case 0:
            newDeviceDriver->read  = (void(*)(uint32_t, uint8_t*)) bus_read_byte;
            newDeviceDriver->write = (void(*)(uint32_t, uint8_t))  bus_write_byte;
            break;
            
        case 1:
            newDeviceDriver->read  = (void(*)(uint32_t, uint8_t*)) bus_read_memory;
            newDeviceDriver->write = (void(*)(uint32_t, uint8_t))  bus_write_memory;
            break;
            
        case 2:
            newDeviceDriver->read  = (void(*)(uint32_t, uint8_t*)) bus_read_byte;
            newDeviceDriver->write = (void(*)(uint32_t, uint8_t))  bus_write_byte;
            break;
        
    }
    
    // Get hardware address pointer
    union Pointer addrPtr;
    addrPtr.byte_t[0] = fileBuffer[DEVICE_NAME_LENGTH + 7];
    addrPtr.byte_t[1] = fileBuffer[DEVICE_NAME_LENGTH + 8];
    addrPtr.byte_t[2] = fileBuffer[DEVICE_NAME_LENGTH + 9];
    addrPtr.byte_t[3] = fileBuffer[DEVICE_NAME_LENGTH + 10];
    
    newDeviceDriver->device.hardware_address = addrPtr.address;
    
    
    // Link the driver to any associated hardware
    // devices if its found the bus
    uint8_t numberOfDevices = GetNumberOfDevices();
    
    for (uint8_t d=0; d < numberOfDevices; d++) {
        
        struct Device* devicePtr = GetDeviceByIndex(d);
        
        if (!StringCompare(devicePtr->device_name, DEVICE_NAME_LENGTH, &fileBuffer[2], DEVICE_NAME_LENGTH)) 
            continue;
        
        newDeviceDriver->is_linked = 1;
        
        newDeviceDriver->device.hardware_slot    = devicePtr->hardware_slot;
        newDeviceDriver->device.hardware_address = devicePtr->hardware_address;
        
        return 2;
    }
    
    return 1;
}

struct Driver* GetDriverByName(uint8_t* name, uint8_t nameLength) {
	uint32_t numberOfDrivers = ListGetSize(DriverTableHead);
	
	for (uint8_t index=0; index < numberOfDrivers; index++) {
        
        struct Driver* driverPtr = GetDriverByIndex(index);
        
        for (uint8_t i=0; i < nameLength - 1; i++) {
            
            if (StringCompare(driverPtr->device.device_name, nameLength, name, nameLength) == 1)
                return driverPtr;
        }
        
        continue;
	}
	
	return nullptr;
}

struct Driver* GetDriverByIndex(uint8_t index) {
    
    struct Node* node = ListGetNode(DriverTableHead, index);
    
    return (struct Driver*)node->data;
}

uint8_t RegisterDriver(void* deviceDriverPtr) {
    
    ListAddNode(&DriverTableHead, deviceDriverPtr);
    
    return ListGetSize(DriverTableHead);
}

uint32_t GetNumberOfDrivers(void) {
    
    return ListGetSize(DriverTableHead);
}


