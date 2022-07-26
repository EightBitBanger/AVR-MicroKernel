//
// Hardware detection

#ifndef __HARDWARE_DETECT_
#define __HARDWARE_DETECT_

// Default system device categories
char _DISPLAY_CONSOLE__[]      = "display";
char _KEYBOARD_INPUT__[]       = "kb";
char _EXTENDED_MEMORY__[]      = "exMem";
char _DATA_MEMORY__[]          = "nvMem";
char _COMMAND_CONSOLE__[]      = "console";
char _INTERNAL_SPEAKER__[]     = "audio";
char _NETWORK_INTERFACE__[]    = "network";

struct HardwareInformation {
	
	char device_id;
	char device_name[10];
	
};


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
	device_bus.waitstate_read  = 20;
	device_bus.waitstate_write = 20;
	
	Device device_driver;
	WrappedPointer pointer;
	HardwareInformation h_info;
	
	for (uint32_t current_address=address; current_address < (address + (stride * device_count)); current_address+=stride) {
		
		// Attempt to recover hardware information from the given address location
		get_hardware_info(current_address, device_bus, h_info);
		
		// Check device exists
		if (get_func_address(h_info.device_name, _DEVICE_NAME_LENGTH_MAX__, device_driver) == 1) {
			
			// Update device address
			pointer.address = current_address;
			call_extern(device_driver, _DEVICE_ADDRESS__, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			
			// Reinitiate the device driver
			call_extern(device_driver, _DEVICE_INITIATE__);
			
			continue;
		}
		
	}
	
	return;
}



#endif

