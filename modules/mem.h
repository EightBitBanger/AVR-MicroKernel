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
	WrappedPointer stackSz;
	WrappedPointer memTotal;
	
	if (getFuncAddress(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memDriverPtr) == 0) return;
	
	callExtern(memDriverPtr, 0x04); // Check total memory
	callExtern(memDriverPtr, 0x0c, memTotal.byte_t[0], memTotal.byte_t[1], memTotal.byte_t[2], memTotal.byte_t[3]);
	
	callExtern(memDriverPtr, 0x03); // Check total stack size
	callExtern(memDriverPtr, 0x0c, stackSz.byte_t[0], stackSz.byte_t[1], stackSz.byte_t[2], stackSz.byte_t[3]);
	
	console.printInt( memTotal.address - stackSz.address );
	console.printLn();
	
	return;
}


