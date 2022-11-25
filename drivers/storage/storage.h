//
// Hardware level storage device driver

#ifndef __STORAGE_DRIVER_
#define __STORAGE_DRIVER_

struct StorageDeviceDriver {
	
	Bus device_bus;
	uint32_t device_address;
	
	StorageDeviceDriver() {
		
		device_bus.waitstate_read  = 2;
		device_bus.waitstate_write = 0;
		
		device_address = 0x00000;
		
	}
	
	void read(uint32_t address, char& buffer) {
		bus_read_byte(device_bus, address, buffer);
		return;
	}
	
	void write(uint32_t address, char& buffer) {
		char check;
		bus_write_byte(device_bus, address, buffer);
		
		bus_read_byte(device_bus, address, check);
		while (check != buffer) 
			bus_read_byte(device_bus, address, check);
		
		return;
	}
	
	
}storageDeviceDriver;









// Device Driver entry point
void storageDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_ADDRESS) {
		WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
		storageDeviceDriver.device_address = pointer.address;
		return;
	}
	
	if (functionCall == 0x00) {storageDeviceDriver.read(storageDeviceDriver.device_address, (char&)paramA); return;}
	if (functionCall == 0x01) {storageDeviceDriver.write(storageDeviceDriver.device_address, (char&)paramA); return;}
	
	return;
}





















#endif

