//
// Device driver table

#ifndef ____DEVICE_DRIVER_SYSTEM__
#define ____DEVICE_DRIVER_SYSTEM__


#define _DRIVER_TABLE_SIZE__         20  // Total number of device drivers
#define _DRIVER_NAME_LENGTH_MAX__     8  // Max string name length

#define _DRIVER_INITIATE__    0xff
#define _DRIVER_SHUTDOWN__    0xfe
#define _DRIVER_ADDRESS__     0xfd

static uint8_t nullchar = 0;

#ifdef  __CORE_MAIN_

typedef void(*Device)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);


// Load a device driver entry point function onto the driver table
uint8_t load_library(const char name[], uint8_t name_length, Device entry_pointer);
// Unload a device driver from the driver table
uint8_t free_library(const char name[], uint8_t name_length);

// Get a library function pointer by its name
uint8_t get_func_address(const char device_name[], uint8_t name_length, Device& entry_pointer);
// Call an external function from a driver entry pointer with the given parameters
uint8_t call_extern(Device& entry_pointer, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD);

// Zero the device driver table
void __extern_initiate(void);
// Initiate all currently loaded device drivers
void __extern_call_init(void);
// Shutdown all currently loaded device drivers
void __extern_call_shutdown(void);


struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][_DRIVER_NAME_LENGTH_MAX__];
	void (*driver_entrypoint_table[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
}volatile static deviceDriverTable;


uint8_t load_library(const char name[], uint8_t name_length, Device entry_pointer) {
	
	if (name_length > _DRIVER_NAME_LENGTH_MAX__) return 0;
	
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	for (uint8_t i=0; i < name_length-1; i++)
		deviceDriverTable.deviceNameIndex[index][i] = name[i];
	
	deviceDriverTable.driver_entrypoint_table[index] = entry_pointer;
	
	return 1;
}


uint8_t free_library(const char name[], uint8_t name_length) {
	
	if (name_length > _DRIVER_NAME_LENGTH_MAX__) return 0;
	
	uint8_t index;
	for (index=0; index < _DRIVER_TABLE_SIZE__; index++) {
		if (deviceDriverTable.driver_entrypoint_table[index] == 0) break; else continue;
	}
	
	for (uint8_t i=0; i < name_length-1; i++)
		deviceDriverTable.deviceNameIndex[index][i] = name[i];
	
	deviceDriverTable.driver_entrypoint_table[index] = 0;
	
	return 1;
}


uint8_t get_func_address(const char device_name[], uint8_t name_length, Device& entry_pointer) {
	
	if (name_length > _DRIVER_NAME_LENGTH_MAX__) return 0;
	
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.deviceNameIndex[i][0] == 0x20) continue;
		
		uint8_t count=0;
		for (uint8_t a=0; a < name_length-1; a++) {
			char nameChar = deviceDriverTable.deviceNameIndex[i][a];
			if (nameChar == device_name[a]) count++; else break;
		}
		
		if (count == name_length-1) {
			entry_pointer = deviceDriverTable.driver_entrypoint_table[i];
			return 1;
		}
	}
	
	return 0;
}


uint8_t call_extern(Device& entry_pointer, uint8_t function_call, uint8_t& paramA=nullchar, uint8_t& paramB=nullchar, uint8_t& paramC=nullchar, uint8_t& paramD=nullchar) {
	
	if (entry_pointer == 0) return 0;
	
	entry_pointer(function_call, paramA, paramB, paramC, paramD);
	
	return 1;
}

#endif

void __extern_call_init() {
	
	#ifdef __CORE_MAIN_
	
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		if ((deviceDriverTable.driver_entrypoint_table[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20)) {
			Device deviceDriver;
			deviceDriver = deviceDriverTable.driver_entrypoint_table[i];
			call_extern(deviceDriver, _DRIVER_INITIATE__);
		}
		
		// Allow time between device initiations
		_delay_ms(50);
	}
	
	#endif
	
}


void __extern_initiate(void) {
	
#ifdef __CORE_MAIN_
	
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		deviceDriverTable.driver_entrypoint_table[i] = 0;
		for (uint8_t a=0; a < _DRIVER_NAME_LENGTH_MAX__; a++)
			deviceDriverTable.deviceNameIndex[i][a] = 0x20;
	}
	
#endif
	
}


void __extern_call_shutdown(void) {
	
#ifdef __CORE_MAIN_
	// Shutdown device drivers
	for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
		if ((deviceDriverTable.driver_entrypoint_table[i] != 0) && (deviceDriverTable.deviceNameIndex[i][0] != 0x20)) {
			Device deviceDriver;
			deviceDriver = deviceDriverTable.driver_entrypoint_table[i];
			call_extern(deviceDriver, _DRIVER_SHUTDOWN__);
		}
	}
#endif
	
}

#endif


