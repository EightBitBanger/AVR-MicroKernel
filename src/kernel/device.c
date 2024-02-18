#include <avr/io.h>

#include <kernel/device.h>


void device_read(struct Device* device, uint32_t offset_address, char* buffer) {
    
    //bus_read_byte(&device->interface, device->hardware_address + offset_address, buffer);
    
    return;
}

void device_write(struct Device* device, uint32_t offset_address, char buffer) {
    
    //bus_write_byte(&device->interface, device->hardware_address + offset_address, buffer);
    
    return;
}


