//
// Device list command module

void command_memory(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "mem"

struct ModuleLoaderMem {
	ModuleLoaderMem() {
		load_library(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)command_memory, _DEVICE_TYPE_MODULE__);
	}
}static loadModuleMem;
#undef __MODULE_NAME_

void command_memory(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device memDriverPtr;
	
	if (get_func_address(_EXTENDED_MEMORY__, sizeof(_EXTENDED_MEMORY__), memDriverPtr) == 0) {
		console.print(error_exmem_not_installed, sizeof(error_exmem_not_installed));
		console.printLn();
		return;
	}
	
	// Check total memory
	call_extern(memDriverPtr, 0x04);
	WrappedPointer memTotal;
	call_extern(memDriverPtr, 0x0c, memTotal.byte_t[0], memTotal.byte_t[1], memTotal.byte_t[2], memTotal.byte_t[3]);
	
	// Check total stack size
	call_extern(memDriverPtr, 0x03);
	WrappedPointer stackSz;
	call_extern(memDriverPtr, 0x0c, stackSz.byte_t[0], stackSz.byte_t[1], stackSz.byte_t[2], stackSz.byte_t[3]);
	
	console.printInt( memTotal.address - stackSz.address );
	console.printLn();
	
	return;
}


