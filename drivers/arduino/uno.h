//
// Arduino UNO board driver

#ifndef _ARDUINO_UNO_DRIVER__
#define _ARDUINO_UNO_DRIVER__

#define _DEVICE_DRIVER_NAME__    "uno"

void arduinoUNODeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct TemplateDeviceDriver {
	
	TemplateDeviceDriver() {
		
		load_device(_DEVICE_DRIVER_NAME__, sizeof(_DEVICE_DRIVER_NAME__), (Device)arduinoUNODeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
	}
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
}static arduinoUNODeviceDriver;


void arduinoUNODeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_INITIATE) {arduinoUNODeviceDriver.initiate(); return;}
	if (functionCall == DEVICE_CALL_SHUTDOWN) {arduinoUNODeviceDriver.shutdown(); return;}
	
	return;
}


#undef _DEVICE_DRIVER_NAME__


#endif

