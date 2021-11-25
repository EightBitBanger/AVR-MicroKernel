//
// Device driver entry point function table

uint8_t loadLibrary(const char[], uint8_t, void(*)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&));
EntryPtr& getFuncAddress(const char[], uint8_t);
uint8_t callExtern(EntryPtr, uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][_DRIVER_TABLE_NAME_SIZE__];
	void (*driver_entrypoint_table[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	DeviceDriverTable() {
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			for (uint8_t a=0; a < _DRIVER_TABLE_NAME_SIZE__; a++) deviceNameIndex[i][a] = 0x20;
			driver_entrypoint_table[i] = (EntryPtr&)nullfunction;
		}
	}
	
};
DeviceDriverTable deviceDriverTable;

// Load a device driver entry point function onto the driver function table
uint8_t loadLibrary(const char device_name[], uint8_t name_length, void(*driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&)) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__) return 0;
	
	// Find a free driver index
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	// Load the library
	for (uint8_t i=0; i < name_length-1; i++) 
		deviceDriverTable.deviceNameIndex[index][i] = device_name[i];
	
	deviceDriverTable.driver_entrypoint_table[index] = driver_ptr;
	
	return 1;
}

// Get a library function address by its device name
EntryPtr& getFuncAddress(const char device_name[], uint8_t name_length) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__) return (EntryPtr&)nullfunction;
	
	// Function look up
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == device_name[a]) count++; else break;
			
			if (count >= name_length) 
				return deviceDriverTable.driver_entrypoint_table[i];
			
		}
		
	}
	
	return (EntryPtr&)nullfunction;
}

// Call an external function from a library function pointer
uint8_t callExtern(EntryPtr library_function, uint8_t function_call, uint8_t& paramA=NULLvariable, uint8_t& paramB=NULLvariable, uint8_t& paramC=NULLvariable, uint8_t& paramD=NULLvariable) {
	
	// Check valid pointer
	if (library_function == (EntryPtr&)nullfunction) return 0;
	
	// Find pointer in the driver table
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.driver_entrypoint_table[i] == library_function) {
			library_function(function_call, paramA, paramB, paramC, paramD);
			return 1;
		}
		
	}
	
	return 1;
}

