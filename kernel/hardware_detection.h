#ifndef __HARDWARE_DETECTION_
#define __HARDWARE_DETECTION_

#define  _HARDWARE_ADDRESS_BEGIN__    0x40000    // Starting slot address
#define  _HARDWARE_ADDRESS_STRIDE__   0x10000    // Address block size per slot
#define  _HARDWARE_SLOT_COUNT__       5          // Number of on board slots


// Recognized device categories
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
void __detect_hardware(void);


void get_hardware_info(uint32_t address, Bus& device_bus, HardwareInformation& h_info) {
	
	bus_read_byte(device_bus, address, h_info.device_id);
	
	for (uint8_t i=0; i < 10; i++) 
		bus_read_byte(device_bus, address + i + 0x01, h_info.device_name[i]);
	
	return;
}


void __detect_hardware(void) {
	
	Bus device_bus;
	
	device_bus.waitstate_read  = 20;
	device_bus.waitstate_write = 20;
	
	Device device_driver;
	WrappedPointer pointer;
	HardwareInformation h_info;
	
	uint32_t address_max = (_HARDWARE_ADDRESS_BEGIN__ + (_HARDWARE_ADDRESS_STRIDE__ * _HARDWARE_SLOT_COUNT__));
	
	for (uint32_t current_address=_HARDWARE_ADDRESS_BEGIN__;
		 current_address < address_max;
		 current_address += _HARDWARE_ADDRESS_STRIDE__) {
		
		get_hardware_info(current_address, device_bus, h_info);
		
		if (get_func_address(h_info.device_name, DEVICE_NAME_LENGTH_MAX, device_driver) == 1) {
			
			pointer.address = current_address;
			call_extern(device_driver, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			
			call_extern(device_driver, DEVICE_CALL_INITIATE);
			
			continue;
		}
		
	}
	
	return;
}



#endif

