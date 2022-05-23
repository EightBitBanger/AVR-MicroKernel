//
// Application test

void command_application__(void);

struct ModuleLoaderTest {
	ModuleLoaderTest() {
		loadModule(command_application__, "app",  4);
	}
}static loadModuleTest;

void executable_task(void);

void command_application__(void) {
	
	DriverEntryPoint memDriverPtr;
	WrappedPointer stackSz;
	WrappedPointer memTotal;
	
	// Driver check
	if (getFuncAddress(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memDriverPtr) == 0) {
		console.print(error_exmem_not_installed, sizeof(error_exmem_not_installed));
		console.printLn();
		return;
	}
	
	createProcess("app", 4, executable_task, 1000000, _TASK_VOLATILE__);
	
	return;
}


void executable_task(void) {
	
	DriverEntryPoint memDriverPtr;
	getFuncAddress(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memDriverPtr);
	
	
	WrappedPointer pointer;
	pointer.address = 0x100;
	callExtern(memDriverPtr, 0x0a, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	callExtern(memDriverPtr, 0x00);
	callExtern(memDriverPtr, 0x0c, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	// Check allocation failed
	if (pointer.address == 0) {
		console.print(error_out_of_memory, sizeof(error_out_of_memory));
		console.printLn();
	}
	
	
	return;
}


