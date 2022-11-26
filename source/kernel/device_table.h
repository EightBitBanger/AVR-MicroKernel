#ifndef __DEVICE_RESOURCE_SYSTEM_
#define __DEVICE_RESOURCE_SYSTEM_

struct DeviceTable {
	
	char    name  [DEVICE_TABLE_SIZE][DEVICE_NAME_LENGTH_MAX];
	uint8_t type  [DEVICE_TABLE_SIZE];
	
	void  (*table [DEVICE_TABLE_SIZE])();
	
}volatile static device_table;


uint8_t load_device(const char* name, uint8_t name_length, void(*device_pointer)(), uint8_t device_type) {
	
	if (name_length > DEVICE_NAME_LENGTH_MAX)
	name_length = DEVICE_NAME_LENGTH_MAX;
	
	uint8_t free_index;
	for (free_index=0; free_index < DEVICE_TABLE_SIZE; free_index++)
	if (device_table.type[free_index] == 0x00) break;
	
	if (free_index == DEVICE_TABLE_SIZE) return 0;
	
	device_table.type[free_index] = device_type;
	
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
		for (i=0; i < name_length-1; i++)
		if (name[i] != device_table.name[index][i]) break;
		
		if (i == name_length-1) {
			
			device_table.type[index] = 0x00;
			
			for (uint8_t i=0; i < DEVICE_NAME_LENGTH_MAX; i++)
			device_table.name[index][i] = 0x20;
			
			device_table.table[index] = 0;
			
		}
		
	}
	
	return 1;
}


void* get_func_address(const char* device_name, uint8_t name_length) {
	
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
		return (void*)device_table.table[i];
		
	}
	
	return (void*) nullptr;
}


void call_extern(Device device_pointer, uint8_t function_call, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
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


