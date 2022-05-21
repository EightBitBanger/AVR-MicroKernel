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
	
	uint8_t  blockSz = 4096;
	uint32_t address = stack_alloc(blockSz);
	
	
	
	
	stack_free(blockSz);
	return;
}


