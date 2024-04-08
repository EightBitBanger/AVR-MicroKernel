#ifndef __DEVICE_TABLE_
#define __DEVICE_TABLE_

#include <kernel/bus/bus.h>

#include <kernel/cstring.h>


#define DEVICE_NAME_LENGTH  10

#define DEVICE_TABLE_SIZE  16

#define DEVICE_REGISTRY_SIZE  24

#define nullptr  0x00000000


struct Device {
    
    uint8_t device_name[DEVICE_NAME_LENGTH];
    
    uint8_t device_id;
    
    uint8_t hardware_slot;
    
    uint32_t hardware_address;
    
};




void InitiateDeviceTable(void);

struct Device* GetHardwareDeviceByIndex(uint8_t index);


uint8_t GetNumberOfDevices(void);

#endif
