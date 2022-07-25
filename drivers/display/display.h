//
// "Liquid Crystal" display card driver

void DisplayDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define _DISPLAY_LINE_WIDTH  20  // Chars per line
#define _DISPLAY_LINE_COUNT   4  // Total lines on display

struct DisplayDriver {
	
	Bus device_bus;
	
	uint32_t device_address;
	
	// Load the device driver
	DisplayDriver() {
		
		device_address = 0x00000;
		
		device_bus.waitstate_read  = 10;
		device_bus.waitstate_write = 10;
		
		load_device(_DISPLAY_CONSOLE__, sizeof(_DISPLAY_CONSOLE__), (Device)DisplayDeviceDriverEntryPoint, _DEVICE_TYPE_DRIVER__);
	}
	
	void initiate(void) {
		
	}
	
	void shutdown(void) {
		
	}
	
	void write(uint32_t address, char byte) {bus_write_byte(device_bus, device_address + address, byte); return;}
	void read(uint32_t address, char& byte) {bus_read_byte(device_bus, device_address + address, byte); return;}
	
	void cursorSetPosition(uint8_t line, uint8_t position) {write(0xa0, line); write(0xa1, position); return;}
	void cursorSetCharacter(uint8_t new_character)         {write(0xa2, new_character); return;}
	void cursorSetBlinkRate(uint8_t blink_rate)            {write(0xa3, blink_rate); return;}
	void cursorSetRefreshRate(uint8_t refresh_rate)        {write(0xa4, refresh_rate); return;}
	
	void clearBuffer(void)       {write(0xa5, 0x01); return;}
	void clearMask(void)         {write(0xa6, 0x01); return;}
	void clearLine(uint8_t line) {write(0xa7, line); return;}
	
	void frameShiftUp(void)   {write(0xa9, 0x01); return;}
	void frameShiftDown(void) {write(0xaa, 0x01); return;}
	
	void writeChar(char character, uint8_t line=0, uint8_t position=0) {write((line * _DISPLAY_LINE_WIDTH) + position, character); return;}
	void writeString(char String[], uint8_t string_size, uint8_t line=0, uint8_t position=0) {
		
		uint32_t address = device_address + ((line * _DISPLAY_LINE_WIDTH) + position);
		
		uint8_t stringSz = string_size - 1;
		
		for (uint8_t i=0; i < stringSz; i++) write(address+i, String[i]);
		
		return;
	}
	void writeString(const char String[], uint8_t string_size, uint8_t line=0, uint8_t position=0) {
		
		uint32_t address = device_address + ((line * _DISPLAY_LINE_WIDTH) + position);
		
		uint8_t stringSz = string_size - 1;
		
		for (uint8_t i=0; i < stringSz; i++) write(address+i, String[i]);
		
		return;
	}
	
	void writeCharMask(const char character, uint8_t line=0, uint8_t position=0) {write(( (line * _DISPLAY_LINE_WIDTH) + position + 0x50), character); return;}
	void writeStringMask(const char string[], uint8_t string_size, uint8_t line=0, uint8_t position=0) {
		
		uint32_t address = device_address + ((line * _DISPLAY_LINE_WIDTH) + position) + 0x50;
		
		uint8_t stringSz = string_size - 1;
		
		for (uint8_t i=0; i < stringSz; i++) write(address+i, string[i]);
		
		return;
	}
	void writeCharGenRam(uint8_t char_number, uint8_t char_line, char character_data) {
		write( ((char_number * 8) + char_line + 0xb0), character_data);
		return;
	}
	
	void uploadCharacterGeneratorRam(uint8_t char_to_upload) {write(0xa8, char_to_upload); return;}
	
}static displayDriver;

// Driver entry point
void DisplayDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DEVICE_INITIATE__: {displayDriver.initiate(); break;}
		case _DEVICE_SHUTDOWN__: {displayDriver.shutdown(); break;}
		case _DEVICE_ADDRESS__: {
			WrappedPointer pointer; pointer.byte_t[0] = paramA; pointer.byte_t[1] = paramB; pointer.byte_t[2] = paramC; pointer.byte_t[3] = paramD;
			displayDriver.device_address = pointer.address;
		}
		
		case 0x00: displayDriver.cursorSetPosition(paramA, paramB); break;
		case 0x01: displayDriver.cursorSetCharacter(paramA); break;
		case 0x02: displayDriver.cursorSetBlinkRate(paramA); break;
		case 0x03: displayDriver.cursorSetRefreshRate(paramA); break;
		
		case 0x04: displayDriver.clearBuffer(); break;
		case 0x05: displayDriver.clearMask(); break;
		case 0x06: displayDriver.clearLine(paramA); break;
		
		case 0x07: displayDriver.frameShiftUp(); break;
		case 0x08: displayDriver.frameShiftDown(); break;
		
		case 0x09: displayDriver.writeChar(paramA, paramB, paramC); break;
		
		default: break;
	}
	
	return;
}


