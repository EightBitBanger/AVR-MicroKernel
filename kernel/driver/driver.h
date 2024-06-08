#ifndef __DRIVER_TABLE_
#define __DRIVER_TABLE_

#include <kernel/bus/bus.h>
#include <kernel/device/device.h>

#include <kernel/cstring.h>

#define DRIVER_REGISTRY_SIZE   24
#define NUMBER_OF_DRIVERS  8


struct Driver {
    
    /// Device associated with this driver
    struct Device device;
    
    /// Low level bus interface
    struct Bus interface;
    
    /// Read a byte from the hardware device
    void(*read)(uint32_t address, uint8_t* buffer);
    
    /// write a byte to the hardware device
    void(*write)(uint32_t address, uint8_t buffer);
    
};


uint8_t LoadLibrary(uint8_t* filename, uint8_t filenameLength);


struct Driver* GetDriverByName(uint8_t* name, uint8_t nameLength);

struct Driver* GetDriverByIndex(uint8_t index);

uint8_t RegisterDriver(void* deviceDriverPtr);

void driverInit(void);


uint8_t GetNumberOfDrivers(void);

#endif
