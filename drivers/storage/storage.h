//
// EEPROM storage driver

#ifndef _DRIVER_MASS_STORAGE__
#define _DRIVER_MASS_STORAGE__

void storageDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct MassStorageDeviceDriver {
	
	Bus device_bus;
	
	uint32_t device_address;
	
	MassStorageDeviceDriver() {
		
		device_address = 0x00000;
		
		device_bus.waitstate_read  = 40;
		device_bus.waitstate_write = 4000;
		
		load_device(_MASS_STORAGE__, sizeof(_MASS_STORAGE__), (Module)storageDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
	}
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
}static massStorageDeviceDriver;


void storageDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_INITIATE) {massStorageDeviceDriver.initiate(); return;}
	if (functionCall == DEVICE_CALL_SHUTDOWN) {massStorageDeviceDriver.shutdown(); return;}
	if (functionCall == DEVICE_CALL_ADDRESS) {
		WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
		massStorageDeviceDriver.device_address = pointer.address;
		return;
	}
	
	if (functionCall == 0x00) {massStorageDeviceDriver.read(massStorageDeviceDriver.device_address, (char&)paramA); return;}
	if (functionCall == 0x01) {massStorageDeviceDriver.write(massStorageDeviceDriver.device_address, (char)paramA); return;}
	
	return;
}


#undef _HARDWARE_WAITSTATE__












#endif

