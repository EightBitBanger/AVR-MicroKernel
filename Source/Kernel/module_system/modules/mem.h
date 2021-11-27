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
	
	uint32_t ammountOfMemory = _STACK_END__ - (_KERNEL_END__ + stack_size());
	
	char memoryAmmount[7];
	intToString(ammountOfMemory, memoryAmmount);
	
	console.print(memoryAmmount, sizeof(memoryAmmount));
	console.printSpace();
	console.print(string_memory_allocator_bytes, sizeof(string_memory_allocator_bytes));
	console.printSpace();
	console.print(string_memory_allocator_free, sizeof(string_memory_allocator_free));
	console.printLn();
	
	return;
}


