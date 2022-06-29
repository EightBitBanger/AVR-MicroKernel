//
// Device driver table

#ifndef ____DEVICE_DRIVER_SYSTEM__
#define ____DEVICE_DRIVER_SYSTEM__


#define _DEVICE_TABLE_SIZE__           50  // Total number of device drivers
#define _DEVICE_NAME_LENGTH_MAX__       8  // Max string name length

#define _DEVICE_INITIATE__           0xff
#define _DEVICE_SHUTDOWN__           0xfe
#define _DEVICE_ADDRESS__            0xfd

#define _DEVICE_TYPE_DRIVER__        0xfc
#define _DEVICE_TYPE_MODULE__        0xfb

static uint8_t nullchar = 0;

typedef void(*Device)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

// Load a device entry point function onto the table
uint8_t load_library(const char name[], uint8_t name_length, Device entry_pointer, uint8_t type);
// Unload a device driver from the table
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
	
	uint8_t device_type[_DEVICE_TABLE_SIZE__];
	char    device_name[_DEVICE_TABLE_SIZE__][_DEVICE_NAME_LENGTH_MAX__];
	
	void (*driver_entrypoint_table[_DEVICE_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
}volatile static deviceDriverTable;


uint8_t load_library(const char name[], uint8_t name_length, Device entry_pointer, uint8_t type) {
	
	if (name_length > _DEVICE_NAME_LENGTH_MAX__) return 0;
	
	uint8_t free_index;
	for (free_index=0; free_index < _DEVICE_TABLE_SIZE__; free_index++) 
		if (deviceDriverTable.device_type[free_index] == 0x00) break;
	
	if (free_index == _DEVICE_TABLE_SIZE__) return 0;
	
	deviceDriverTable.device_type[free_index] = type;
	
	for (uint8_t i=0; i < name_length-1; i++)
		deviceDriverTable.device_name[free_index][i] = name[i];
	
	if (entry_pointer == nullptr) return 0;
	deviceDriverTable.driver_entrypoint_table[free_index] = entry_pointer;
	
	return 1;
}


uint8_t free_library(const char name[], uint8_t name_length) {
	
	uint8_t nameLength = name_length - 1;
	if (name_length > _DEVICE_NAME_LENGTH_MAX__) return 0;
	
	for (uint8_t index=0; index < _DEVICE_TABLE_SIZE__; index++) {
		
		uint8_t i;
		for (i=0; i < nameLength; i++) 
			if (name[i] != deviceDriverTable.device_name[index][i]) break;
		
		if (i == nameLength) {
			
			deviceDriverTable.device_type[index] = 0x00;
			
			for (uint8_t i=0; i < _DEVICE_NAME_LENGTH_MAX__; i++)
				deviceDriverTable.device_name[index][i] = 0x20;
			
			deviceDriverTable.driver_entrypoint_table[index] = 0;
			
		} else {
			return 0;
		}
		
	}
	
	return 1;
}


uint8_t get_func_address(const char device_name[], uint8_t name_length, Device& entry_pointer) {
	
	if (name_length > _DEVICE_NAME_LENGTH_MAX__) return 0;
	
	for (uint8_t i=0; i < _DEVICE_TABLE_SIZE__; i++) {
		
		if (deviceDriverTable.device_name[i][0] == 0x20) continue;
		
		uint8_t count=0;
		for (uint8_t a=0; a < name_length-1; a++) {
			char nameChar = deviceDriverTable.device_name[i][a];
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



void __extern_call_init() {
#ifdef __CORE_MAIN_
	for (uint8_t i=0; i < _DEVICE_TABLE_SIZE__; i++) {
		if ((deviceDriverTable.device_type[i] == _DEVICE_TYPE_DRIVER__) && (deviceDriverTable.driver_entrypoint_table[i] != nullptr)) {
			Device deviceDriver;
			deviceDriver = deviceDriverTable.driver_entrypoint_table[i];
			call_extern(deviceDriver, _DEVICE_INITIATE__);
		}
	}
#endif
}


void __extern_call_shutdown(void) {
#ifdef __CORE_MAIN_
	for (uint8_t i=0; i < _DEVICE_TABLE_SIZE__; i++) {
		if ((deviceDriverTable.device_type[i] == _DEVICE_TYPE_DRIVER__) && (deviceDriverTable.driver_entrypoint_table[i] != nullptr)) {
			Device deviceDriver;
			deviceDriver = deviceDriverTable.driver_entrypoint_table[i];
			call_extern(deviceDriver, _DEVICE_SHUTDOWN__);
		}
	}
#endif
	
}

void __extern_initiate(void) {
#ifdef __CORE_MAIN_
	for (uint8_t i=0; i < _DEVICE_TABLE_SIZE__; i++) {
		
		deviceDriverTable.device_type[i] = 0x00;
		
		for (uint8_t a=0; a < _DEVICE_NAME_LENGTH_MAX__; a++)
			deviceDriverTable.device_name[i][a] = 0x20;
		
		deviceDriverTable.driver_entrypoint_table[i] = 0;
		
	}
#endif
}



#endif




