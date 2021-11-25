//
// Device list command module

void command_memory(void);

struct ModuleMemoryCommand {
	
	ModuleMemoryCommand() {
		
		const char command_name[] = "mem";
		loadModule(&command_memory, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleMemoryCommand moduleMemoryCommand;

void command_memory(void) {
	
	uint32_t ammountOfMemory = _STACK_END__ - (_KERNEL_END__ + allocator.stack_size());
	
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


