// Hardware detection

#ifndef __HARDWARE_DETECT_
#define __HARDWARE_DETECT_

struct HardwareInformation {
	char id;
	char name[10];
};


// Get information on hardware starting at the given address
void get_hardware_info(Bus device_bus, uint32_t address, HardwareInformation& h_info);









void get_hardware_info(Bus device_bus, uint32_t address, HardwareInformation& h_info) {
	
	// Get device ID byte
	bus_read_byte(device_bus, address, h_info.id);
	
	// Get device name 10 bytes
	for (uint8_t i=1; i < 10; i++) 
		bus_read_byte(device_bus, address + i, h_info.name[i]);
	
	
}








#endif

