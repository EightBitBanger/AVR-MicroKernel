//
// Device resource table

#ifndef ____DEVICE_RESOURCE_SYSTEM__
#define ____DEVICE_RESOURCE_SYSTEM__


#define _DEVICE_TABLE_SIZE__           30  // Device table size
#define _DEVICE_NAME_LENGTH_MAX__       8  // Device name length

#define _DEVICE_INITIATE__           0xff
#define _DEVICE_SHUTDOWN__           0xfe
#define _DEVICE_ADDRESS__            0xfd

#define _DEVICE_TYPE_DRIVER__        0xfc
#define _DEVICE_TYPE_LIBRARY__       0xfb
#define _DEVICE_TYPE_MODULE__        0xfa

static uint8_t nullchar = 0;

typedef void(*Device)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);


// Load a device onto the device table
uint8_t load_device(const char name[], uint8_t name_length, Device device_pointer, uint8_t type);

// Unload a device from the table
uint8_t free_device(const char name[], uint8_t name_length);

// Get a device entry pointer by its name (Note: slow performance)
uint8_t get_func_address(const char device_name[], uint8_t name_length, Device& device_pointer);

// Call a function pointer with the given parameters
uint8_t call_extern(Device& entry_pointer, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD);


struct DeviceTable {
	
	uint8_t device_type[_DEVICE_TABLE_SIZE__];
	char    device_name[_DEVICE_TABLE_SIZE__][_DEVICE_NAME_LENGTH_MAX__];
	
	void (*device_table[_DEVICE_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
}volatile static deviceTable;


uint8_t load_device(const char name[], uint8_t name_length, Device device_pointer, uint8_t type) {
	
	if (name_length > _DEVICE_NAME_LENGTH_MAX__)
		name_length = _DEVICE_NAME_LENGTH_MAX__;
	
	uint8_t free_index;
	for (free_index=0; free_index < _DEVICE_TABLE_SIZE__; free_index++) 
		if (deviceTable.device_type[free_index] == 0x00) break;
	
	if (free_index == _DEVICE_TABLE_SIZE__) return 0;
	
	deviceTable.device_type[free_index] = type;
	
	for (uint8_t i=0; i < name_length-1; i++)
		deviceTable.device_name[free_index][i] = name[i];
	
	if (device_pointer == nullptr) return 0;
	deviceTable.device_table[free_index] = device_pointer;
	
	return 1;
}


uint8_t free_device(const char name[], uint8_t name_length) {
	
	if (name_length > _DEVICE_NAME_LENGTH_MAX__)
		name_length = _DEVICE_NAME_LENGTH_MAX__;
	
	for (uint8_t index=0; index < _DEVICE_TABLE_SIZE__; index++) {
		
		uint8_t i;
		for (i=0; i < name_length; i++) 
			if (name[i] != deviceTable.device_name[index][i]) break;
		
		if (i == name_length) {
			
			deviceTable.device_type[index] = 0x00;
			
			for (uint8_t i=0; i < _DEVICE_NAME_LENGTH_MAX__; i++)
				deviceTable.device_name[index][i] = 0x20;
			
			deviceTable.device_table[index] = 0;
			
		} else {
			return 0;
		}
		
	}
	
	return 1;
}


uint8_t get_func_address(const char device_name[], uint8_t name_length, Device& device_pointer) {
	
	if (name_length > _DEVICE_NAME_LENGTH_MAX__) 
		name_length = _DEVICE_NAME_LENGTH_MAX__;
	
	for (uint8_t i=0; i < _DEVICE_TABLE_SIZE__; i++) {
		
		if (deviceTable.device_name[i][0] == 0x20) continue;
		
		uint8_t count=0;
		for (uint8_t a=0; a < name_length-1; a++) {
			char nameChar = deviceTable.device_name[i][a];
			if (nameChar == device_name[a]) count++; else break;
		}
		
		if (count == name_length-1) {
			device_pointer = deviceTable.device_table[i];
			return 1;
		}
	}
	
	return 0;
}


uint8_t call_extern(Device& entry_pointer, uint8_t function_call, uint8_t& paramA=nullchar, uint8_t& paramB=nullchar, uint8_t& paramC=nullchar, uint8_t& paramD=nullchar) {
	
	entry_pointer(function_call, paramA, paramB, paramC, paramD);
	
	return 1;
}



void __extern_call_init() {
	for (uint8_t i=0; i < _DEVICE_TABLE_SIZE__; i++) {
		if ((deviceTable.device_type[i] == _DEVICE_TYPE_DRIVER__) && (deviceTable.device_table[i] != 0)) {
			Device deviceDriver;
			deviceDriver = deviceTable.device_table[i];
			call_extern(deviceDriver, _DEVICE_INITIATE__);
		}
		_delay_us(100);
	}
}


void __extern_call_shutdown(void) {
	for (uint8_t i=0; i < _DEVICE_TABLE_SIZE__; i++) {
		if ((deviceTable.device_type[i] == _DEVICE_TYPE_DRIVER__) && (deviceTable.device_table[i] != 0)) {
			Device deviceDriver;
			deviceDriver = deviceTable.device_table[i];
			call_extern(deviceDriver, _DEVICE_SHUTDOWN__);
		}
		_delay_ms(100);
	}
}

void __extern_initiate(void) {
	for (uint8_t i=0; i < _DEVICE_TABLE_SIZE__; i++) {
		
		deviceTable.device_type[i] = 0x00;
		
		for (uint8_t a=0; a < _DEVICE_NAME_LENGTH_MAX__; a++)
			deviceTable.device_name[i][a] = 0x20;
		
		deviceTable.device_table[i] = 0;
		
	}
}



#endif




