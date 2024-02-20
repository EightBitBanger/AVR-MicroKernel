#ifndef __DEVICE_DRIVER_
#define __DEVICE_DRIVER_

#include <kernel/bus/bus.h>
#include <kernel/strcase.h>

#define PERIPHERAL_ADDRESS_BEGIN  0x40000
#define PERIPHERAL_STRIDE         0x10000

#define NUMBER_OF_PERIPHERALS  5

#define DEVICE_NAME_LENGTH  10

#define DEVICE_TABLE_SIZE  16



struct Device {
    
    char device_name[DEVICE_NAME_LENGTH];
    
    uint32_t hardware_address;
    
};

void initiateDeviceTable(void);

struct Device* GetDevice(unsigned int deviceIndex);

#endif
