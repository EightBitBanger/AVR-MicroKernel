#include <avr/io.h>
#include "driver_table.h"

struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][_DRIVER_TABLE_NAME_SIZE__];
	void (*driver_entrypoint_table[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
};
DeviceDriverTable deviceDriverTable;

void driver_table_initiate(void) {
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		for (uint8_t a=0; a < _DRIVER_TABLE_NAME_SIZE__; a++)
			deviceDriverTable.deviceNameIndex[i][a] = 0x20;
		
		deviceDriverTable.driver_entrypoint_table[i] = 0;
	}
}

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

uint8_t callExtern(DriverEntryPoint& entry_pointer, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (entry_pointer == 0) return 0;
	
	entry_pointer(function_call, paramA, paramB, paramC, paramD);
	
	return 1;
}

