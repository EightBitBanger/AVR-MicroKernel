#ifndef ____DEVICE_RESOURCE_SYSTEM__
#define ____DEVICE_RESOURCE_SYSTEM__


// Device table characteristics
#define DEVICE_TABLE_SIZE           30  // Device table size
#define DEVICE_NAME_LENGTH_MAX       8  // Device name length
// Device function calls
#define DEVICE_CALL_INITIATE      0xff  // Called when the kernel is started
#define DEVICE_CALL_SHUTDOWN      0xfe  // Called when the kernel is shutdown
#define DEVICE_CALL_ADDRESS       0xfd  // Hardware address for the device on the bus
// Device types
#define DEVICE_TYPE_DRIVER        0xfc  // Hardware level IO device
#define DEVICE_TYPE_LIBRARY       0xfb  // Function library
#define DEVICE_TYPE_MODULE        0xfa  // Command line module


typedef void(*Module)();
typedef void(*Driver)();
typedef void(*Device)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

// Load a device onto the device table
uint8_t load_device(const char* name, uint8_t name_length, void(*device_pointer)(), uint8_t type);
// Unload a device from the table
uint8_t free_device(const char* name, uint8_t name_length);
// Get a device entry pointer by its name (Note: slow)
Module get_func_address(const char* device_name, uint8_t name_length);
// Call a device function pointer with the given parameter(s)
void call_extern(Device device_pointer, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD);


// Call device initiation functions
void __extern_call_init(void);
// Call device shutdown functions
void __extern_call_shutdown(void);
// Initiate / clear the device table
void __extern_initiate(void);


struct DeviceTable {
	
	char    name  [DEVICE_TABLE_SIZE][DEVICE_NAME_LENGTH_MAX];
	uint8_t type  [DEVICE_TABLE_SIZE];
	
	void  (*table [DEVICE_TABLE_SIZE])();
	
}volatile static device_table;


uint8_t load_device(const char* name, uint8_t name_length, void(*device_pointer)(), uint8_t type) {
	
	if (name_length > DEVICE_NAME_LENGTH_MAX)
		name_length = DEVICE_NAME_LENGTH_MAX;
	
	uint8_t free_index;
	for (free_index=0; free_index < DEVICE_TABLE_SIZE; free_index++) 
		if (device_table.type[free_index] == 0x00) break;
	
	if (free_index == DEVICE_TABLE_SIZE) return 0;
	
	device_table.type[free_index] = type;
	
	for (uint8_t i=0; i < name_length-1; i++)
		device_table.name[free_index][i] = name[i];
	
	if (device_pointer == 0) return 0;
	device_table.table[free_index] = device_pointer;
	
	return 1;
}


uint8_t free_device(const char* name, uint8_t name_length) {
	
	if (name_length > DEVICE_NAME_LENGTH_MAX)
		name_length = DEVICE_NAME_LENGTH_MAX;
	
	for (uint8_t index=0; index < DEVICE_TABLE_SIZE; index++) {
		
		uint8_t i;
		for (i=0; i < name_length; i++) 
			if (name[i] != device_table.name[index][i]) break;
		
		if (i == name_length) {
			
			device_table.type[index] = 0x00;
			
			for (uint8_t i=0; i < DEVICE_NAME_LENGTH_MAX; i++)
				device_table.name[index][i] = 0x20;
			
			device_table.table[index] = 0;
			
		} else {
			return 0;
		}
		
	}
	
	return 1;
}


Module get_func_address(const char* device_name, uint8_t name_length) {
	
	if (name_length > DEVICE_NAME_LENGTH_MAX)
		name_length = DEVICE_NAME_LENGTH_MAX;
	
	for (uint8_t i=0; i < DEVICE_TABLE_SIZE; i++) {
		
		if (device_table.name[i][0] == 0x20) continue;
		
		uint8_t count=0;
		for (uint8_t a=0; a < name_length-1; a++) {
			char nameChar = device_table.name[i][a];
			if (nameChar == device_name[a]) count++; else break;
		}
		
		if (count == name_length-1)
		return (Module)device_table.table[i];
		
	}
	
	return (Module) nullptr;
}


void call_extern(Device device_pointer, uint8_t function_call, uint8_t& paramA=nullchar, uint8_t& paramB=nullchar, uint8_t& paramC=nullchar, uint8_t& paramD=nullchar) {
	
	device_pointer(function_call, paramA, paramB, paramC, paramD);
	
	return;
}



void __extern_call_init(void) {
#ifdef __CORE_MAIN_
	
	for (uint8_t i=0; i < DEVICE_TABLE_SIZE; i++) {
		
		if ((device_table.type[i] == DEVICE_TYPE_DRIVER) && (device_table.table[i] != 0)) {
			void(*deviceDriver)(uint8_t) = (void(*)(uint8_t))device_table.table[i];
			deviceDriver(DEVICE_CALL_INITIATE);
		}
		
	}
	
#endif
}

void __extern_call_shutdown(void) {
#ifdef __CORE_MAIN_
	
	for (uint8_t i=0; i < DEVICE_TABLE_SIZE; i++) {
		
		if ((device_table.type[i] == DEVICE_TYPE_DRIVER) && (device_table.table[i] != 0)) {
			void(*deviceDriver)(uint8_t) = (void(*)(uint8_t))device_table.table[i];
			deviceDriver(DEVICE_CALL_SHUTDOWN);
		}
		
	}
	
#endif
}

void __extern_initiate(void) {
#ifdef __CORE_MAIN_
	
	for (uint8_t i=0; i < DEVICE_TABLE_SIZE; i++) {
		
		device_table.type[i] = 0x00;
		for (uint8_t a=0; a < DEVICE_NAME_LENGTH_MAX; a++)
			device_table.name[i][a] = 0x20;
		
		device_table.table[i] = 0;
	}
	
#endif
}



#endif



