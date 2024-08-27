#ifndef __DEVICE_TABLE_
#define __DEVICE_TABLE_

#include <kernel/bus/bus.h>
#include <kernel/cstring.h>

#include <kernel/kconfig.h>


#define DEVICE_NAME_LENGTH       10

#define DEVICE_TABLE_SIZE        24


struct Device {
    
    /// Name of the device
    uint8_t device_name[DEVICE_NAME_LENGTH];
    
    /// Software interrupt ID
    uint8_t device_id;
    
    /// Physical peripheral slot where
    /// the device is located
    uint8_t hardware_slot;
    
    /// Hardware address pointing to the 
    /// device on the bus
    uint32_t hardware_address;
    
};


void initiateDeviceTable(void);

struct Device* GetHardwareDeviceByIndex(uint8_t index);

uint8_t GetNumberOfDevices(void);

#endif
