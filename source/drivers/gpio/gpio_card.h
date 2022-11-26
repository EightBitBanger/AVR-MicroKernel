//
// GPIO card driver

#ifndef __GPIO_CARD__
#define __GPIO_CARD__

struct GPIOCardDriver {
	
	Bus device_bus;
	
	uint32_t portAddress;
	
	GPIOCardDriver() {
		
		portAddress = 0x60000;
		
		device_bus.waitstate_read  = 8;
		device_bus.waitstate_write = 8;
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
}static GPIOCardDriver;




// Driver entry point
void GPIOCardDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	WrappedPointer pointer;
	
	if (functionCall == 0x00) {GPIOCardDriver.write(GPIOCardDriver.portAddress + paramA, paramB); return;}
	
	return;
}





#endif


