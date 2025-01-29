#ifndef __DRIVER_TABLE_
#define __DRIVER_TABLE_

#include <kernel/bus/bus.h>

#include <kernel/device/device.h>


#define DRIVER_REGISTRY_SIZE   8

#define DRIVER_TABLE_SIZE  16


struct Driver {
    
    /// Device associated with this driver
    struct Device device;
    
    /// Is the driver currently linked to a hardware device
    uint8_t is_linked;
    
    /// Low level IO interface
    void (*read)(uint32_t address, uint8_t* buffer);
    void (*write)(uint32_t address, uint8_t buffer);
    
};


int8_t LoadLibrary(uint8_t* filename, uint8_t filenameLength);

struct Driver* GetDriverByName(uint8_t* name, uint8_t nameLength);

struct Driver* GetDriverByIndex(uint8_t index);

uint8_t RegisterDriver(void* deviceDriverPtr);

void DriverTableInit(void);


uint8_t GetNumberOfDrivers(void);

#endif
