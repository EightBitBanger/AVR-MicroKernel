//
// Device list command module

void command_memory(void);

struct ModuleMemoryCommand {
	
	ModuleMemoryCommand() {
		
		const char command_name[] = "mem";
		kernel.function.install(&command_memory, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleMemoryCommand moduleMemoryCommand;

void command_memory(void) {
	
	uint32_t ammountOfMemory = (_STACK_END__ - _STACK_BEGIN__) - _KERNEL_END__;
	string memoryAmmount(7);
	intToString(ammountOfMemory, memoryAmmount);
	
	string memoryBytes(string_memory_allocator_bytes, sizeof(string_memory_allocator_bytes));
	string memoryFree(string_memory_allocator_free, sizeof(string_memory_allocator_free));
	
	console.print(memoryAmmount, memoryAmmount.size());
	console.print(memoryBytes, memoryBytes.size());
	console.print(memoryFree, memoryFree.size());
	console.newLine();
	
	return;
}


