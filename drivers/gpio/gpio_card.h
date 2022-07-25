//
// GPIO card

#ifndef __GPIO_CARD__
#define __GPIO_CARD__

void GPIOCardDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);


struct GPIOCardDriver {
	
	Bus device_bus;
	
	uint32_t portAddress;
	
	GPIOCardDriver() {
		
		portAddress = 0x60000;
		
		device_bus.waitstate_read  = 8;
		device_bus.waitstate_write = 8;
		
		load_device("GPIO", sizeof("GPIO"), (Device)GPIOCardDeviceDriverEntryPoint, _DEVICE_TYPE_DRIVER__);
		
	}
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
}static GPIOCardDriver;




// Driver entry point
void GPIOCardDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	WrappedPointer pointer;
	
	switch(functionCall) {
		
		case _DEVICE_INITIATE__: {GPIOCardDriver.initiate(); break;}
		case _DEVICE_SHUTDOWN__: {GPIOCardDriver.shutdown(); break;}
		
		case 0x00: {GPIOCardDriver.write(GPIOCardDriver.portAddress + paramA, paramB); break;}
		
		default: break;
	}
	
	return;
}





#endif


