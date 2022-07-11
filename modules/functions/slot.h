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
	uint8_t index;
	uint32_t address;
	HardwareInformation hinfo;
	
	index = console.keyboard_string[5] - 0x30;
	
	address = 0x40000 + (index * 0x10000);
	
	device_bus.waitstate_write = 40;
	device_bus.waitstate_read  = 40;
	
	get_hardware_info(address, device_bus, hinfo);
	
	console.print(hinfo.device_name, 10);
	console.printLn();
	
	return;
}

#undef __MODULE_NAME_

