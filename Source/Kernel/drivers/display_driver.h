//
// Liquid Crystal display card driver

#define _DEVICE_DISPLAY__    0x10

struct DisplayDriver {
	
	void write(uint32_t address, char byte) {
		uint32_t deviceAddress = device.getAddress(_DEVICE_DISPLAY__);
		device_write((deviceAddress+address), byte);
		return;
	}
	void read(uint32_t address, char& byte) {
		uint32_t deviceAddress = device.getAddress(_DEVICE_DISPLAY__);
		device_read((deviceAddress+address), byte);
		return;
	}
	
	void cursorSetPosition(uint8_t line, uint8_t position) {write(0xa0, line); write(0xa1, position); return;}
	void cursorSetCharacter(uint8_t new_character) {write(0xa2, new_character); return;}
	void cursorSetBlinkRate(uint8_t blink_rate) {write(0xa3, blink_rate); return;}
	void cursorSetRefreshRate(uint8_t refresh_rate) {write(0xa4, refresh_rate); return;}
	
	void clearBuffer(void) {write(0xa5, 0x01); return;}
	void clearMask(void) {write(0xa6, 0x01); return;}
	void clearLine(uint8_t line) {write(0xa7, line); return;}
	
	void frameShiftUp(void) {write(0xa9, 0x01); return;}
	void frameShiftDown(void) {write(0xaa, 0x01); return;}
	
	void writeChar(char character, uint8_t line=0, uint8_t position=0) {write((line * 20) + position, character); return;}
	void writeString(const char string[], uint8_t string_size, uint8_t line=0, uint8_t position=0) {
		
		uint32_t deviceAddress = device.getAddress(_DEVICE_DISPLAY__);
		uint32_t address = deviceAddress + ((line * 20) + position);
		
		uint8_t stringSz = string_size - 1;
		
		for (uint8_t i=0; i < stringSz; i++) device_write(address+i, string[i]);
		
		return;
	}
	void writeCharMask(const char character, uint8_t line=0, uint8_t position=0) {write(( (line * 20) + position + 0x50), character); return;}
	void writeStringMask(const char string[], uint8_t string_size, uint8_t line=0, uint8_t position=0) {
		
		uint32_t deviceAddress = device.getAddress(_DEVICE_DISPLAY__);
		uint32_t address = deviceAddress + ((line * 20) + position) + 0x50;
		
		uint8_t stringSz = string_size - 1;
		
		for (uint8_t i=0; i < stringSz; i++) device_write(address+i, string[i]);
		
		return;
	}
	void writeCharGenRam(uint8_t char_number, uint8_t char_line, char character_data) {
		write( ((char_number * 8) + char_line + 0xb0), character_data);
		return;
	}
	
	void uploadCharacterGeneratorRam(uint8_t char_to_upload) {write(0xa8, char_to_upload); return;}
	
};
DisplayDriver displayDriver;


