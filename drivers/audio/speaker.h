//
// On-board speaker

#ifndef __INTERNAL__SPEAKER__
#define __INTERNAL__SPEAKER__

void SpeakerDriverDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);


struct SpeakerDriver {
	
	Bus device_bus;
	
	uint32_t portAddress;
	uint32_t baseFreq;
	
	SpeakerDriver() {
		
		portAddress = 0xc0000;
		
		baseFreq    = 1000;
		
		// Load the device driver
		load_library("Speaker", sizeof("Speaker"), (Device)SpeakerDriverDeviceDriverEntryPoint);
	}
	
	void initiate(void) {
		
		char byte;
		
		device_bus.waitstate_read  = 0;
		device_bus.waitstate_write = 0;
		
		beep(50, 1);
		_delay_ms(300);
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, address, byte); return;}
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, address, byte); return;}
	
	void beep(uint8_t  length, uint8_t tone) {
		
		uint32_t baseDelta  = baseFreq + (100 * tone);
		uint32_t baseLength = length * 2;
		
		for (uint32_t i=0; i < baseLength; i++) {
			
			bus_write_byte(device_bus, portAddress, 0x01);
			for (uint32_t t=0; t<baseDelta; t++) asm("nop");
			
			bus_write_byte(device_bus, portAddress, 0x00);
			for (uint32_t t=0; t<baseDelta; t++) asm("nop");
			
		}
		
	}
	
}static speakerDriver;




// Driver entry point
void SpeakerDriverDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	WrappedPointer pointer;
	
	switch(functionCall) {
		
		case _DRIVER_INITIATE__: {speakerDriver.initiate(); break;}
		case _DRIVER_SHUTDOWN__: {speakerDriver.shutdown(); break;}
		
		case 0x00: {speakerDriver.beep(paramA, paramB); break;}
		
		default: break;
	}
	
	return;
}





#endif


