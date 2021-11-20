//
// Device list command module

void command_list_devices(void);

struct ModuleDeviceList {
	
	ModuleDeviceList() {
		
		// Command name
		char name[] = "device";
		
		// Install the function pointer
		kernel.installFunction(&command_list_devices, name, sizeof(name));
		
		return;
	}
	
};
static ModuleDeviceList moduleDeviceList;

void command_list_devices(void) {
	
	// Clear the buffer and mask
	displayDriver.clearBuffer();
	displayDriver.clearMask();
	
	_delay_ms(100);
	
	return;
}


