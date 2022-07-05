// Hardware detection

#ifndef __HARDWARE_DETECT_
#define __HARDWARE_DETECT_


struct HardwareInformation {
	
	char device_id;
	char device_name[10];
	
};

// Detection functions
uint8_t checkDisplayDevice(uint32_t	address, Device device_driver, HardwareInformation h_info);


// Get information on hardware starting at the given address
void get_hardware_info(uint32_t address, Bus& device_bus, HardwareInformation& h_info);

// Detect attached hardware
//   address         Beginning device location
//   stride          Next device offset location
//   device_count    Number of offset device locations to iterate through
void __detect_hardware(uint32_t address, uint32_t stride, uint8_t device_count);


void get_hardware_info(uint32_t address, Bus& device_bus, HardwareInformation& h_info) {
	
	// Get device ID byte
	bus_read_byte(device_bus, address, h_info.device_id);
	
	// Get device name 10 bytes
	for (uint8_t i=0; i < 10; i++) 
		bus_read_byte(device_bus, address + i + 0x01, h_info.device_name[i]);
	
	return;
}

void __detect_hardware(uint32_t address, uint32_t stride, uint8_t device_count) {
	
	// External bus interface
	Bus device_bus;
	device_bus.waitstate_read  = 10;
	device_bus.waitstate_write = 10;
	
	Device device_driver;
	HardwareInformation h_info;
	
	for (uint32_t current_address=address; current_address < (address + (stride * device_count)); current_address+=stride) {
		
		// Attempt to recover hardware information from the given address location
		get_hardware_info(current_address, device_bus, h_info);
		
		if (checkDisplayDevice(current_address, device_driver, h_info) == 1) continue;
		
		
		//
		// Network interface device
		if (string_compare(h_info.device_name, _NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__)) == 1) {
			
			if (get_func_address(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), device_driver) == 0) continue;
			
			// Update device address
			WrappedPointer pointer;
			pointer.address = current_address;
			
			call_extern(device_driver, _DEVICE_ADDRESS__, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			
			// Re-initiate the command console
			if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), device_driver) == 0) continue;
			
			call_extern(device_driver, _DEVICE_INITIATE__);
		}
		
	}
	
	return;
}

uint8_t checkDisplayDevice(uint32_t current_address, Device device_driver, HardwareInformation h_info) {
	
	if (string_compare(h_info.device_name, _DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__)) == 1) {
		
		if (get_func_address(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), device_driver) == 0) return 0;
		
		// Update device address
		WrappedPointer pointer;
		pointer.address = current_address;
		
		call_extern(device_driver, _DEVICE_ADDRESS__, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		
		// Re-initiate the command console
		if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), device_driver) == 0) return 0;
		
		call_extern(device_driver, _DEVICE_INITIATE__);
	}
	
	return 1;
}




#endif

