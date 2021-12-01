//
// Device driver system

#define _DRIVER_TABLE_SIZE__         16  // Total number of elements
#define _DRIVER_TABLE_NAME_SIZE__    16  // Max name length

// Driver entry pointer wrapper
struct Device {
	
	void(*EntryPoint)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	Device() {
		
		EntryPoint = (void(*)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&))&nullfunction;
		
	}
	
	
};

// Load a device driver entry point function onto the driver table
uint8_t loadLibrary(const char name[], uint8_t name_length, void(*driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&));
// Get a library function address by its device name
uint8_t getFuncAddress(const char device_name[], uint8_t name_length, Device& device);
// Call an external function from a library function pointer
uint8_t callExtern(Device& device, uint8_t function_call, uint8_t& paramA=NULL, uint8_t& paramB=NULL, uint8_t& paramC=NULL, uint8_t& paramD=NULL);

struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][_DRIVER_TABLE_NAME_SIZE__];
	void (*driver_entrypoint_table[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	DeviceDriverTable() {
		
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++)
		for (uint8_t a=0; a < _DRIVER_TABLE_NAME_SIZE__; a++)
		deviceNameIndex[i][a] = 0x20;
		
	}
	
};
DeviceDriverTable deviceDriverTable;

uint8_t loadLibrary(const char name[], uint8_t name_length, void(*driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&)) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__-1) return 0;
	
	// Find a free driver index
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	// Load the library
	for (uint8_t i=0; i < name_length-1; i++)
	deviceDriverTable.deviceNameIndex[index][i] = name[i];
	
	deviceDriverTable.driver_entrypoint_table[index] = driver_ptr;
	
	return 1;
}

uint8_t getFuncAddress(const char device_name[], uint8_t name_length, Device& device) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__) return 0;
	
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == device_name[a]) count++; else break;
		}
		
		if (count == name_length) {
			device.EntryPoint = deviceDriverTable.driver_entrypoint_table[i];
			return 1;
		}
	}
	
	return 0;
}

uint8_t callExtern(Device& device, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	// Check valid pointer
	if (device.EntryPoint == (EntryPtr&)NULL_f) return 0;
	
	device.EntryPoint(function_call, paramA, paramB, paramC, paramD);
	
	return 1;
}


