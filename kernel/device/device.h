#ifndef __DEVICE_TABLE_
#define __DEVICE_TABLE_

#include <kernel/bus/bus.h>

#include <kernel/strcase.h>

#define PERIPHERAL_ADDRESS_BEGIN  0x40000
#define PERIPHERAL_STRIDE         0x10000

#define NUMBER_OF_PERIPHERALS  5

#define DEVICE_NAME_LENGTH  10

#define DEVICE_TABLE_SIZE  16

#define DEVICE_REGISTRY_SIZE  24

#define nullptr  0x00000000


struct Device {
    
    char device_name[DEVICE_NAME_LENGTH];
    
    uint8_t device_id;
    
    uint32_t hardware_address;
    
};




void InitiateDeviceTable(void);

struct Device* GetHardwareDeviceByIndex(uint8_t index);


uint8_t GetNumberOfDevices(void);

#endif
