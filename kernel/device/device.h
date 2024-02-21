#ifndef __DEVICE_DRIVER_
#define __DEVICE_DRIVER_

#include <kernel/bus/bus.h>
#include <kernel/strcase.h>

#define PERIPHERAL_ADDRESS_BEGIN  0x40000
#define PERIPHERAL_STRIDE         0x10000

#define NUMBER_OF_PERIPHERALS  5

#define DEVICE_NAME_LENGTH  10

#define DEVICE_TABLE_SIZE  16

#define DEVICE_REGISTRY_SIZE  24

struct Device {
    
    char device_name[DEVICE_NAME_LENGTH];
    
    uint8_t device_id;
    
    uint32_t hardware_address;
    
};


struct Driver {
    
    struct Device device;
    
    struct Bus interface;
    
    void(*read)(uint32_t address, char* buffer);
    void(*write)(uint32_t address, char buffer);
    
};



void InitiateDeviceTable(void);

struct Device* GetHardwareDevice(unsigned int deviceIndex);

uint8_t RegisterDriver(void* deviceDriverPtr);

#endif
