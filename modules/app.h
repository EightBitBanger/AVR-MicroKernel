//
// Application test

void command_application__(void);

void test(void);


struct ModuleLoaderTest {
	ModuleLoaderTest() {
		loadModule(command_application__, "app",  4);
	}
};
ModuleLoaderTest loadModuleTest;

void command_application__(void) {
	
	createProcess("app", 4, test, 1000000, _TASK_VOLATILE__);
	
	return;
}


void test(void) {
	
	DriverEntryPoint consoleDriver;
	if (getFuncAddress(_COMMAND_CONSOLE__, sizeof(_COMMAND_CONSOLE__), consoleDriver) == 0) return;
	
	DriverEntryPoint memDriverPtr;
	if (getFuncAddress(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memDriverPtr) == 0) return;
	
	WrappedPointer pointer;
	pointer.address = 4096;
	callExtern(memDriverPtr, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	callExtern(memDriverPtr, 0x00);
	callExtern(memDriverPtr, 0x0c, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	// Check allocation failed
	if (pointer.address == 0) {
		console.print(error_out_of_memory, sizeof(error_out_of_memory));
	}
	
	
	return;
}


