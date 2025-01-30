#include <stdint.h>

#include <kernel/delay.h>
#include <kernel/cstring.h>

#include <kernel/device/device.h>
#include <kernel/driver/driver.h>

struct Node* DeviceTableHead = NULL;


void InitiateDeviceTable(void) {
    
    struct Bus bus;
    
    bus.read_waitstate  = 20;
    bus.write_waitstate = 20;
    
    //
    // Initiate peripheral devices
    
    unsigned int index=0;
    
    for (uint32_t d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        // Allow time between device initiation
        _delay_us(100);
        
        // Calculate device address offset
        uint32_t hardware_address = PERIPHERAL_ADDRESS_BEGIN + (PERIPHERAL_STRIDE * d);
        
        struct Device* newDevicePtr = (struct Device*)malloc(sizeof(struct Device));
        ListAddNode(&DeviceTableHead, newDevicePtr);
        
        // Get device name
        for (uint8_t i=0; i < DEVICE_NAME_LENGTH; i++) 
            bus_read_byte(&bus, hardware_address + i + 1, &newDevicePtr->device_name[i]);
        
        // Blank the name once a space character is found
        uint8_t checkFinished = 0;
        for (uint8_t i=0; i < DEVICE_NAME_LENGTH; i++) {
            
            if (newDevicePtr->device_name[i] == ' ') 
                checkFinished = 1;
            
            if (checkFinished != 0) 
                newDevicePtr->device_name[i] = ' ';
        }
        
        // Get device ID
        bus_read_byte(&bus, hardware_address, &newDevicePtr->device_id);
        
        // Reject device name
        if (is_letter(&newDevicePtr->device_name[0]) == 0) 
            continue;
        
        if (newDevicePtr->device_id == 0) 
            continue;
        
        newDevicePtr->hardware_address = hardware_address;
        
        newDevicePtr->hardware_slot = d;
        
        index++;
        
        continue;
    }
    
    
    //
    // Link the hardware to their associated device drivers
    
    // Check the registered drivers
    for (unsigned int i=0; i < GetNumberOfDrivers(); i++) {
        
        uint32_t numberOfDevices = ListGetSize(DeviceTableHead);
        
        // Look up the device to link the driver
        for (unsigned int d=0; d < numberOfDevices; d++) {
            
            struct Node* node = ListGetNode(DeviceTableHead, d);
            struct Device* devicePtr = (struct Device*)node->data;
            
            // Get the device from the driver
            struct Driver* driver = GetDriverByIndex(i);
            struct Device* device = (struct Device*)GetDriverByIndex(i);
            
            // Check hardware IDs
            if (device->device_id != devicePtr->device_id) 
                continue;
            
            // Link the hardware addresses
            device->hardware_slot = devicePtr->hardware_slot;
            
            device->hardware_address = devicePtr->hardware_address;
            
            driver->is_linked = 1;
            
            break;
        }
        
    }
    
    return;
}


struct Device* GetDeviceByIndex(uint8_t index) {
    
    return ListGetNode(DeviceTableHead, index)->data;
}


uint8_t GetNumberOfDevices(void) {
    
    return ListGetSize(DeviceTableHead);
}
