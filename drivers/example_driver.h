//
// Example device driver library

void DeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

// Installer
struct LibraryInstaller {
	LibraryInstaller() {
		loadLibrary(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), &DeviceDriverEntryPoint);
	}
};
LibraryInstaller libraryInstaller;

struct DeviceDriver {
	
	void initiate(void) {
		
		return;
	}
	
	void shutdown(void) {
		
		return;
	}
	
	void example_function(void) {
		
		return;
	}
	
};
DeviceDriver deviceDriver;

void DeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DRIVER_INITIATE__: {deviceDriver.initiate(); break;}
		case _DRIVER_SHUTDOWN__: {deviceDriver.shutdown(); break;}
		
		case 0x00: deviceDriver.example_function(); break;
		
		default: break;
	}
	
	return;
}


