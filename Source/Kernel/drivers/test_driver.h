//
// Liquid crystal display driver

void displayLibraryEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD);

struct DisplayLibrary {
	
	DisplayLibrary() {
		
		const char command_name[] = "con";
		kernel.loadLibrary(command_name, sizeof(command_name), &displayLibraryEntryPoint);
		
	}
	
	void write(uint32_t address, char byte) {device_write(device.getAddress(_DEVICE_DISPLAY__) + address, byte); return;}
	void read(uint32_t address, char& byte) {device_read(device.getAddress(_DEVICE_DISPLAY__) + address, byte); return;}
	
	void clearDisplay(void) {
		
		// Clear the buffer and mask
		displayDriver.clearBuffer();
		displayDriver.clearMask();
		kernel.console.consoleClearString();
		
		// Reset the cursor
		kernel.console.cursorLine  = 0;
		kernel.console.cursorPos   = 1;
		
		_delay_ms(200);
		
		// Initiate the prompt
		kernel.console.consoleNewPrompt();
		
		kernel.console.promptState = 1;
		
	}
	
};
DisplayLibrary displayLibrary;

void displayLibraryEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case 0x01: displayLibrary.clearDisplay(); break;
		case 0x05: paramA = 100; break;
		
		default: break;
	}
	
}


