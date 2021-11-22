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
	
	string memoryFree(string_memory_allocator, sizeof(string_memory_allocator));
	
	console.addString(memoryFree, memoryFree.size());
	console.newLine();
	//console.
	
	return;
}


