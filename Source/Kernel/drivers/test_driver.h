//
// Test driver

void displayLibraryEntryPoint(uint8_t functionCall, uint8_t& functionParam);

struct DisplayLibrary {
	
	DisplayLibrary() {
		
		const char command_name[] = "con";
		kernel.driver.loadLibrary(&displayLibraryEntryPoint, command_name);
		
	}
	
	
	void write(uint32_t address, char byte) {device_write(device.getAddress(_DEVICE_DISPLAY__) + address, byte); return;}
	void read(uint32_t address, char& byte) {device_read(device.getAddress(_DEVICE_DISPLAY__) + address, byte); return;}
	
	
	
};
DisplayLibrary displayLibrary;

void displayLibraryEntryPoint(uint8_t functionCall, uint8_t& functionParam) {
	
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


