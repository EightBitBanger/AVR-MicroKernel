//
// Device slot detection

void slot_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
void slot_task(void);

#define __MODULE_NAME_  "slot"

struct ModuleLoaderSlot {
	
	ModuleLoaderSlot() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)slot_entry_point, _DEVICE_TYPE_MODULE__);
	}
}static slotModuleLoader;



void slot_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Bus device_bus;
	HardwareInformation hinfo;
	
	uint8_t param0  = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1  = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	
	// Slot query selection
	if ((param0 >= '1') & (param0 <= '5')) {
		
		uint32_t address = 0x30000 + ((param0 - '0') * 0x10000);
		
		device_bus.waitstate_write = 20;
		device_bus.waitstate_read  = 20;
		
		get_hardware_info(address, device_bus, hinfo);
		
		console.print(hinfo.device_name, 10);
		console.printLn();
		return;
	}
	
	console.print(error_device_not_found, sizeof(error_device_not_found));
	console.printLn();
	
	return;
}

#undef __MODULE_NAME_

