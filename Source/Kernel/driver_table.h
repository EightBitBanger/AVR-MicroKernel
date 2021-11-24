//
// Device driver entry point function table

uint8_t loadLibrary(const char[], uint8_t, void(*)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&));
FunctionPtr& getFuncAddress(const char[], uint8_t);
uint8_t callExtern(FunctionPtr, uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][8];
	void (*driver_entrypoint_table[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	DeviceDriverTable() {
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			for (uint8_t a=0; a < 8; a++) deviceNameIndex[i][a] = 0x20;
			driver_entrypoint_table[i] = 0;
		}
	}
	
};
DeviceDriverTable deviceDriverTable;

// Load device drivers
#include "drivers.h"

// Load a device driver entry point function onto the driver function table
uint8_t loadLibrary(const char device_name[], uint8_t name_length, void(*new_driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&)) {
	
	if (name_length > 7) return 0;
	
	// Find a free driver index
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	// Load the library
	for (uint8_t i=0; i < name_length; i++) deviceDriverTable.deviceNameIndex[index][i] = device_name[i];
	deviceDriverTable.driver_entrypoint_table[index] = new_driver_ptr;
	
	return 1;
}

// Get a library function address by its device name
FunctionPtr& getFuncAddress(const char device_name[], uint8_t name_length) {
	
	if (name_length > 7) return (FunctionPtr&)nullfunction;
	
	// Function look up
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.driver_entrypoint_table[i] == 0x00) continue;
		
		uint8_t count=0;
		for (count=0; count < name_length; count++)
		if (deviceDriverTable.deviceNameIndex[i][count] == device_name[count]) {continue;} else {count=0; break;}
		
		if (count == name_length) return deviceDriverTable.driver_entrypoint_table[i];
	}
	
	return (FunctionPtr&)nullfunction;
}

// Call an external function from a library function pointer
uint8_t callExtern(FunctionPtr library_function, uint8_t function_call, uint8_t& paramA=NULL, uint8_t& paramB=NULL, uint8_t& paramC=NULL, uint8_t& paramD=NULL) {
	
	if (library_function == (FunctionPtr&)nullfunction) return 0;
	
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.driver_entrypoint_table[i] == library_function) {
			library_function(function_call, paramA, paramB, paramC, paramD);
			return 1;
		}
		
	}
	
	return 1;
}

