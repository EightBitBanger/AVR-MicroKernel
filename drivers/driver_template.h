//
// Example device driver template

#ifndef _DRIVER_TEMPLATE__
#define _DRIVER_TEMPLATE__

#define _DEVICE_DRIVER_NAME__    "template"
#define _HARDWARE_WAITSTATE__    10

void templateDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct TemplateDeviceDriver {
	
	Bus device_bus;
	
	uint32_t device_address;
	
	TemplateDeviceDriver() {
		
		device_address = 0x00000;
		
		device_bus.waitstate_read  = _HARDWARE_WAITSTATE__;
		device_bus.waitstate_write = _HARDWARE_WAITSTATE__;
		
		load_device(_DEVICE_DRIVER_NAME__, sizeof(_DEVICE_DRIVER_NAME__), (Device)templateDeviceDriverEntryPoint, _DEVICE_TYPE_DRIVER__);
	}
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
}static templateDeviceDriver;


void templateDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DEVICE_INITIATE__: {templateDeviceDriver.initiate(); break;}
		case _DEVICE_SHUTDOWN__: {templateDeviceDriver.shutdown(); break;}
		case _DEVICE_ADDRESS__: {
			WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
			templateDeviceDriver.device_address = pointer.address;
		}
		
		case 0x00: {templateDeviceDriver.read(templateDeviceDriver.device_address, (char&)paramA); break;}
		case 0x01: {templateDeviceDriver.write(templateDeviceDriver.device_address, (char)paramA); break;}
		
		default: break;
	}
	
	return;
}


#undef _HARDWARE_WAITSTATE__












#endif

