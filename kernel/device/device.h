#ifndef __DEVICE_TABLE_
#define __DEVICE_TABLE_

#include <kernel/bus/bus.h>

#include <kernel/cstring.h>


#define DEVICE_NAME_LENGTH       10

#define DEVICE_TABLE_SIZE        24

#define nullptr                  0x00000000


#include <kernel/configuration.h>

struct Device {
    
    // Name of the device
    uint8_t device_name[DEVICE_NAME_LENGTH];
    
    // Software interrupt device ID
    uint8_t device_id;
    
    // Physical peripheral slot where
    // the device is located
    uint8_t hardware_slot;
    
    // Hardware address pointing to the 
    // device on the bus
    uint32_t hardware_address;
    
    // Hardware address offset from "hardware_address"
    // pointing to the initialization firmware 
    // if any is available. 0 = No firmware
    uint32_t firmware_address;
    
};


void InitiateDeviceTable(void);

struct Device* GetHardwareDeviceByIndex(uint8_t index);

uint8_t GetNumberOfDevices(void);

#endif
