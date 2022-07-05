//
// Network interface client card

#ifndef __NETWORK_INTERFACE_CARD__
#define __NETWORK_INTERFACE_CARD__

void NetworkInterfaceDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);


struct NetworkInterfaceDriver {
	
	Bus device_bus;
	
	uint32_t portAddress;
	
	NetworkInterfaceDriver() {
		
		portAddress = 0x00000;
		
		load_device(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), (Device)NetworkInterfaceDeviceDriverEntryPoint, _DEVICE_TYPE_DRIVER__);
	}
	
	void initiate(void) {
		
		char byte;
		
		device_bus.waitstate_read  = 8;
		device_bus.waitstate_write = 8;
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
}static networkInterfaceDriver;




// Driver entry point
void NetworkInterfaceDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	WrappedPointer pointer;
	
	switch(functionCall) {
		
		case _DEVICE_INITIATE__: {networkInterfaceDriver.initiate(); break;}
		case _DEVICE_SHUTDOWN__: {networkInterfaceDriver.shutdown(); break;}
		
		//case 0x00: {networkInterfaceDriver.write(networkInterfaceDriver.portAddress + paramA, paramB); break;}
		
		default: break;
	}
	
	return;
}





#endif


