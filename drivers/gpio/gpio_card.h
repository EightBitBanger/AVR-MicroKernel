//
// GPIO card driver

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
		
		load_device("GPIO", sizeof("GPIO"), (void(*)())GPIOCardDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
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
	
	if (functionCall == DEVICE_CALL_INITIATE) {GPIOCardDriver.initiate(); return;}
	if (functionCall == DEVICE_CALL_SHUTDOWN) {GPIOCardDriver.shutdown(); return;}
	if (functionCall == 0x00) {GPIOCardDriver.write(GPIOCardDriver.portAddress + paramA, paramB); return;}
	
	return;
}





#endif


