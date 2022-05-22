//
// Device driver system

#ifndef __DEVICE_DRIVER_SYSTEM__
#define __DEVICE_DRIVER_SYSTEM__

#define _DRIVER_TABLE_SIZE__         20  // Total number of running drivers
#define _DRIVER_TABLE_NAME_SIZE__    10  // Max string name length

#define _DRIVER_INITIATE__  0xff
#define _DRIVER_SHUTDOWN__  0xfe

static uint8_t nullchar =0;

struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][_DRIVER_TABLE_NAME_SIZE__];
	void (*driver_entrypoint_table[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	DeviceDriverTable() {
		
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			
			driver_entrypoint_table[i] = 0;
			
			for (uint8_t a=0; a < _DRIVER_TABLE_NAME_SIZE__; a++)
			deviceNameIndex[i][a] = 0x20;
		}
		
	}
	
}static deviceDriverTable;



typedef void(*DriverEntryPoint)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);



// Load a device driver entry point function onto the driver table
uint8_t loadLibrary(const char name[], uint8_t name_length, DriverEntryPoint entry_pointer) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__-1) return 0;
	
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	for (uint8_t i=0; i < name_length-1; i++)
	deviceDriverTable.deviceNameIndex[index][i] = name[i];
	
	deviceDriverTable.driver_entrypoint_table[index] = entry_pointer;
	
	return 1;
}

// Unload a device driver from the driver table
uint8_t freeLibrary(const char name[], uint8_t name_length) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__-1) return 0;
	
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	for (uint8_t i=0; i < name_length-1; i++)
	deviceDriverTable.deviceNameIndex[index][i] = name[i];
	
	deviceDriverTable.driver_entrypoint_table[index] = 0;
	
	return 1;
}

// Get a library function pointer by its name
uint8_t getFuncAddress(const char device_name[], uint8_t name_length, DriverEntryPoint& entry_pointer) {
	
	if (name_length > _DRIVER_TABLE_NAME_SIZE__) return 0;
	
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		uint8_t count=1;
		for (uint8_t a=0; a < name_length; a++) {
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == device_name[a]) count++; else break;
		}
		
		if (count == name_length) {
			entry_pointer = deviceDriverTable.driver_entrypoint_table[i];
			return 1;
		}
	}
	
	return 0;
}

// Call an external function from a driver entry pointer
uint8_t callExtern(DriverEntryPoint& entry_pointer, uint8_t function_call, uint8_t& paramA=nullchar, uint8_t& paramB=nullchar, uint8_t& paramC=nullchar, uint8_t& paramD=nullchar) {
	
	if (entry_pointer == 0) return 0;
	
	entry_pointer(function_call, paramA, paramB, paramC, paramD);
	
	return 1;
}

// Initiate loaded device drivers
void extern_initiate(void) {
	
	// Load device drivers
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		if ((deviceDriverTable.driver_entrypoint_table[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20)) {
			DriverEntryPoint driverEntryPoint;
			driverEntryPoint = deviceDriverTable.driver_entrypoint_table[i];
			callExtern(driverEntryPoint, _DRIVER_INITIATE__);
		}
		_delay_ms(50);
	}
	
}

// Shutdown loaded device drivers
void extern_shutdown(void) {
	
	// Shutdown device drivers
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		if ((deviceDriverTable.driver_entrypoint_table[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20)) {
			DriverEntryPoint driverEntryPoint;
			driverEntryPoint = deviceDriverTable.driver_entrypoint_table[i];
			callExtern(driverEntryPoint, _DRIVER_SHUTDOWN__);
		}
	}
	
}

#endif


