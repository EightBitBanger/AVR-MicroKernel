//
// Application example

void command_application__(void);

void test(void);


struct ModuleLoaderTest {
	ModuleLoaderTest() {
		loadModule(command_application__, "app",  4);
	}
};
ModuleLoaderTest loadModuleTest;

void command_application__(void) {
	
	createProcess("app", 4, test, 1000000, _TASK_USER__);
	
	// Clear the buffer and mask
	//console.clearBuffer();
	//console.clearMask();
	//console.clearString();
	
	//console.setCursorPosition(0, 0);
	//console.setCursorBlinkRate(0);
	
	// Draw a line
	//console.setCursorPosition(1, 0);
	//for (uint8_t i=0; i<20; i++) console.printChar('=');
	
	//const char FileName[] = "file_name";
	//console.print(FileName, sizeof(FileName));
	
	
	/*
	bool isRunning=true;
	while(isRunning) {
		
		
		
		if (console.lastChar == 0x00)
			isRunning = false;
		
	}
	*/
	
	
	// Reset the cursor
	//console.cursorLine  = 0;
	//console.cursorPos   = 1;
	
	//console.clearBuffer();
	//console.clearMask();
	//console.clearString();
	
	//console.setCursorPosition(0, 0);
	//console.setCursorBlinkRate(0);
	
	return;
}


void test(void) {
	
	DriverEntryPoint consoleDriver;
	DriverEntryPoint memoryDriver;
	
	if (getFuncAddress(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	if (getFuncAddress("ExMem", 6, memoryDriver) == 0) return;
	
	callExtern(memoryDriver, 0x00, (uint8_t&)"A");
	
	
	
	//callExtern(consoleDriver, 0x00, (uint8_t&)"A");
	
	WrappedPointer pointer;
	pointer.address = 0x00020;
	callExtern(memoryDriver, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	callExtern(memoryDriver, 0x00);
	
	callExtern(memoryDriver, 0x0c, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	
}


