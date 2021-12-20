//
// Device driver abstraction

#ifndef __DEVICE_DRIVER_SYSTEM__
#define __DEVICE_DRIVER_SYSTEM__

#define _DRIVER_TABLE_SIZE__         10  // Total number of running drivers
#define _DRIVER_TABLE_NAME_SIZE__    10  // Max string name length

// Referenceable default value
static uint8_t nullvalue = 0;

typedef void(*DriverEntryPoint)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

// Initiate the driver table system
void driver_table_initiate(void);

// Load a device driver entry point function onto the driver table
uint8_t loadLibrary(const char name[], uint8_t name_length, DriverEntryPoint entry_pointer);

// Unload a device driver from the driver table
uint8_t freeLibrary(const char name[], uint8_t name_length);

// Get a library function pointer by its name
uint8_t getFuncAddress(const char name[], uint8_t name_length, DriverEntryPoint& entry_pointer);

// Call an external function from a driver entry pointer
uint8_t callExtern(DriverEntryPoint& entry_pointer, uint8_t function_call, uint8_t& paramA=nullvalue, uint8_t& paramB=nullvalue, uint8_t& paramC=nullvalue, uint8_t& paramD=nullvalue);


#endif


