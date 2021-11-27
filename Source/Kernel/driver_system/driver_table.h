//
// Device driver entry point function table

struct DeviceDriverTable {
	
	char deviceNameIndex[_DRIVER_TABLE_SIZE__][_DRIVER_TABLE_NAME_SIZE__];
	void (*driver_entrypoint_table[_DRIVER_TABLE_SIZE__])(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
	
	DeviceDriverTable() {
		for (uint8_t i=0; i < _DRIVER_TABLE_SIZE__; i++) {
			for (uint8_t a=0; a < _DRIVER_TABLE_NAME_SIZE__; a++) deviceNameIndex[i][a] = 0x20;
		}
	}
	
};
DeviceDriverTable deviceDriverTable;

uint8_t loadLibrary(const char name[], uint8_t name_length, void(*driver_ptr)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&));
EntryPtr& getFuncAddress(const char device_name[], uint8_t name_length);
uint8_t callExtern(EntryPtr& library_function, uint8_t function_call, uint8_t& paramA=NULL, uint8_t& paramB=NULL, uint8_t& paramC=NULL, uint8_t& paramD=NULL);


