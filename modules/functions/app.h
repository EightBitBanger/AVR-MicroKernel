//
// Application test

void command_application__(void);

struct ModuleLoaderTest {
	ModuleLoaderTest() {
		load_module("app",  4, command_application__);
	}
}static loadModuleTest;

void executable_task(void);

void command_application__(void) {
	
	DriverEntryPoint memDriverPtr;
	DriverEntryPoint consoleDriverPtr;
	
	// Check driver dependency
	if (get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memDriverPtr) == 0) {
		console.print(error_exmem_not_installed, sizeof(error_exmem_not_installed));
		console.printLn();
		return;
	}
	
	task_create("app", 4, executable_task, 1000000, _TASK_VOLATILE__);
	
	return;
}


void executable_task(void) {
	
	//DriverEntryPoint memDriverPtr;
	//getFuncAddress(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memDriverPtr);
	
	
	
	
	/*
	
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
	
	*/
	
	return;
}


