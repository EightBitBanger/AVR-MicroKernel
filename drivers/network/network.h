//
// Network interface client card driver

#ifndef __NETWORK_INTERFACE_CARD__
#define __NETWORK_INTERFACE_CARD__

void NetworkInterfaceDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);


struct NetworkInterfaceDriver {
	
	Bus device_bus;
	
	uint32_t device_address;
	
	NetworkInterfaceDriver() {
		
		device_address = 0x00000;
		
		load_device(_NETWORK_INTERFACE__, sizeof(_NETWORK_INTERFACE__), (Device)NetworkInterfaceDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
	}
	
	void initiate(void) {
		
		device_bus.waitstate_read  = 10;
		device_bus.waitstate_write = 10;
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
	// TX flag
	void writeTXflag(uint8_t flag) {write(device_address + 0x0a, flag);}
	void readTXflag(uint8_t& flag) {read(device_address + 0x0a, (char&)flag);}
	// RX flag
	void writeRXflag(uint8_t flag) {write(device_address + 0x0b, flag);}
	void readRXflag(uint8_t& flag) {read(device_address + 0x0b, (char&)flag);}
	
	// TX frame buffer
	void writeTXbuffer(uint8_t frame_data) {write(device_address + 0x0c, frame_data);}
	void readTXbuffer(uint8_t& frame_data) {read(device_address + 0x0c, (char&)frame_data);}
	
	// Setup baud rate
	void writeBaudrateFlag(uint8_t flag) {write(device_address + 0x0d, flag);}
	void writeBaudrate(uint8_t baudrate)   {write(device_address + 0x0e, baudrate);}
	void readBaudrate(uint8_t& baudrate)   {read(device_address + 0x0e, (char&)baudrate);}
	
	// RX frame buffer array
	void writeRXbuffer(uint8_t index, uint8_t frame_data) {write(device_address + 0x10 + index, frame_data);}
	void readRXbuffer(uint8_t index, uint8_t& frame_data) {read(device_address + 0x10 + index, (char&)frame_data);}
	
}static networkInterfaceDriver;




// Device Driver entry point
void NetworkInterfaceDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_INITIATE) {networkInterfaceDriver.initiate(); return;}
	if (functionCall == DEVICE_CALL_SHUTDOWN) {networkInterfaceDriver.shutdown(); return;}
	if (functionCall == DEVICE_CALL_ADDRESS) {
		WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
		networkInterfaceDriver.device_address = pointer.address;
		return;
	}
	
	if (functionCall == 0x00) {networkInterfaceDriver.writeTXflag(paramA); return;}
	if (functionCall == 0x01) {networkInterfaceDriver.readTXflag(paramA); return;}
	
	if (functionCall == 0x02) {networkInterfaceDriver.writeRXflag(paramA); return;}
	if (functionCall == 0x03) {networkInterfaceDriver.readRXflag(paramA); return;}
	
	if (functionCall == 0x04) {networkInterfaceDriver.writeTXbuffer(paramA); return;}
	if (functionCall == 0x05) {networkInterfaceDriver.readTXbuffer(paramA); return;}
	
	if (functionCall == 0x06) {networkInterfaceDriver.writeBaudrateFlag(paramA); return;}
	if (functionCall == 0x07) {networkInterfaceDriver.writeBaudrate(paramA); return;}
	
	if (functionCall == 0x09) {networkInterfaceDriver.writeRXbuffer(paramA, paramB); return;}
	if (functionCall == 0x0a) {networkInterfaceDriver.readRXbuffer(paramA, paramB); return;}
	
	return;
}





#endif


