#ifndef __CALL_EXTERN_H___
#define __CALL_EXTERN_H___

typedef void(*Device)(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

// Call a device function pointer with the given parameter(s)
void call_extern(Device device_pointer, uint8_t function_call, uint8_t& paramA=nullchar, uint8_t& paramB=nullchar, uint8_t& paramC=nullchar, uint8_t& paramD=nullchar) {
	
	device_pointer(function_call, paramA, paramB, paramC, paramD);
	
	return;
}


#endif

