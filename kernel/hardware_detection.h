#ifndef __HARDWARE_DETECTION_
#define __HARDWARE_DETECTION_

#define  _HARDWARE_ADDRESS_BEGIN__    0x40000    // Starting address of the first peripheral
#define  _HARDWARE_ADDRESS_STRIDE__   0x10000    // Address width per peripheral slot
#define  _HARDWARE_SLOT_COUNT__       5          // Number of peripheral slots

// Recognized device categories
char _DISPLAY_DEVICE__[]       = "display";
char _COMMAND_CONSOLE__[]      = "console";
char _KEYBOARD_INPUT__[]       = "kb";
char _EXTENDED_MEMORY__[]      = "exmem";
char _DATA_MEMORY__[]          = "nvmem";
char _INTERNAL_SPEAKER__[]     = "audio";
char _NETWORK_INTERFACE__[]    = "network";
char _MASS_STORAGE__[]         = "storage";
char _GPIO_INTERFACE__[]       = "gpio";

struct HardwareInformation {
	
	char device_id;
	
	char device_name[10];
	
};

struct HardwareDescriptorTable {
	
	char     device_id     [16];
	uint32_t device_address[16];
	
} hardware_descriptor_table;

uint32_t get_hardware_address(uint8_t hardware_id) {
	for (uint8_t i=0; i < _HARDWARE_SLOT_COUNT__; i++) {
		if (hardware_descriptor_table.device_id[i] == hardware_id) 
			return hardware_descriptor_table.device_address[i];
	}
	return 0;
}


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
	
	
	
	uint32_t address_max = (_HARDWARE_ADDRESS_BEGIN__ + (_HARDWARE_ADDRESS_STRIDE__ * _HARDWARE_SLOT_COUNT__));
	uint8_t slot_counter = 0;
	
	for (uint32_t current_address=_HARDWARE_ADDRESS_BEGIN__;
		 current_address < address_max;
		 current_address += _HARDWARE_ADDRESS_STRIDE__) {
		
		HardwareInformation h_info;
		get_hardware_info(current_address, device_bus, h_info);
		
		Device device_driver = (Device)get_func_address(h_info.device_name, DEVICE_NAME_LENGTH_MAX);
		if (device_driver != 0) {
			
			WrappedPointer pointer;
			pointer.address = current_address;
			call_extern(device_driver, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			
			hardware_descriptor_table.device_id[slot_counter]      = h_info.device_id;
			hardware_descriptor_table.device_address[slot_counter] = current_address;
			
			call_extern(device_driver, DEVICE_CALL_INITIATE);
			
			continue;
		}
		
		slot_counter++;
	}
	
	return;
}


uint8_t device_check_header(char* device_name, uint32_t device_address) {
	
	Bus device_bus;
	device_bus.waitstate_read  = 2;
	device_bus.waitstate_write = 0;
	
	HardwareInformation hInfo;
	get_hardware_info(device_address, device_bus, hInfo);
	
	if (strcmp(hInfo.device_name, device_name, 8) == 1)
		return 1;
	
	return 0;
}


#endif

