#ifndef __DEVICE_DEFINITIONS_
#define __DEVICE_DEFINITIONS_

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
uint8_t load_device(const char* name, uint8_t name_length, void(*device_pointer)(), uint8_t device_type);
// Unload a device from the table
uint8_t free_device(const char* name, uint8_t name_length);
// Get a device entry pointer by its name (Note: slow)
void* get_func_address(const char* device_name, uint8_t name_length);
// Call a device function pointer with the given parameter(s)
void call_extern(Device device_pointer, uint8_t function_call, uint8_t& paramA=nullchar, uint8_t& paramB=nullchar, uint8_t& paramC=nullchar, uint8_t& paramD=nullchar);

// Call device initiation functions
void __extern_call_init(void);
// Call device shutdown functions
void __extern_call_shutdown(void);
// Initiate / clear the device table
void __extern_initiate(void);

#endif

