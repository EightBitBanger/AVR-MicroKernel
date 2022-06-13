// Hardware detection

#ifndef __HARDWARE_DETECT_
#define __HARDWARE_DETECT_


struct HardwareInformation {
	
	char device_id;
	char device_name[10];
	
};


// Get information on hardware starting at the given address
void get_hardware_info(Bus& device_bus, uint32_t address, HardwareInformation& h_info);

// Detect attached hardware
//   address         Beginning device location
//   stride          Next device offset location
//   device_count    Number of offset device locations to iterate through
void __detect_hardware(uint32_t address, uint32_t stride, uint8_t device_count);








void get_hardware_info(Bus& device_bus, uint32_t address, HardwareInformation& h_info) {
#ifdef __HARDWARE_AUTO_DETECT_
	// Get device ID byte
	bus_read_byte(device_bus, address, h_info.device_id);
	
	// Get device name 10 bytes
	for (uint8_t i=0; i < 10; i++) 
		bus_read_byte(device_bus, address + i + 0x01, h_info.device_name[i]);
#endif	
}

void __detect_hardware(uint32_t address, uint32_t stride, uint8_t device_count) {
#ifdef __HARDWARE_AUTO_DETECT_
	Bus device_bus;
	device_bus.waitstate_read  = 10;
	device_bus.waitstate_write = 10;
	
	HardwareInformation h_info;
	
	for (uint32_t current_address=address; current_address < (address + (stride * device_count)); current_address+=stride) {
		
		get_hardware_info(device_bus, current_address, h_info);
		Device device_driver;
		
		// Update display device
		if (string_compare(h_info.device_name, _DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__)) == 1) {
			
			if (get_func_address(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), device_driver) == 0) continue;
			
			// Update device address
			WrappedPointer pointer;
			pointer.address = current_address;
			
			call_extern(device_driver, _DRIVER_ADDRESS__, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			
			// Re-initiate the command console
			if (get_func_address(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), device_driver) == 0) continue;
			
			call_extern(device_driver, _DRIVER_INITIATE__);
		}
		
	}
#endif
}






#endif

