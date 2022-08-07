//
// Arduino UNO board driver

#ifndef _DRIVER_TEMPLATE__
#define _DRIVER_TEMPLATE__

#define _DEVICE_DRIVER_NAME__    "uno"

void arduinoUNODeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct TemplateDeviceDriver {
	
	TemplateDeviceDriver() {
		load_device(_DEVICE_DRIVER_NAME__, sizeof(_DEVICE_DRIVER_NAME__), (Device)arduinoUNODeviceDriverEntryPoint, _DEVICE_TYPE_DRIVER__);
	}
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
}static arduinoUNODeviceDriver;


void arduinoUNODeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DEVICE_INITIATE__: {arduinoUNODeviceDriver.initiate(); break;}
		case _DEVICE_SHUTDOWN__: {arduinoUNODeviceDriver.shutdown(); break;}
		
		//case 0x00: {break;}
		
		default: break;
	}
	
	return;
}


#undef _HARDWARE_WAITSTATE__












#endif

