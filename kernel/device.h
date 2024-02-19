#ifndef __DEVICE_DRIVER_
#define __DEVICE_DRIVER_

#include <kernel/bus.h>

#define DEVICE_NAME_LENGTH  10


struct Device {
    
    char device_name[DEVICE_NAME_LENGTH];
    
    uint32_t hardware_address;
    
};


#endif
