//
// Device list command module

void command_memory(void);

struct ModuleLoaderMem {
	ModuleLoaderMem() {
		loadModule(command_memory, "mem",  4);
	}
};
ModuleLoaderMem loadModuleMem;

void command_memory(void) {
	
	DriverEntryPoint memDriverPtr;
	WrappedPointer pointer;
	
	if (getFuncAddress("ExMem", 6, memDriverPtr) == 0) return;
	
	callExtern(memDriverPtr, 0x03); // Check total stack size
	//callExtern(memDriverPtr, 0x04); // Check total memory
	callExtern(memDriverPtr, 0x0c, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	console.printInt( pointer.address );
	console.printLn();
	
	return;
}


