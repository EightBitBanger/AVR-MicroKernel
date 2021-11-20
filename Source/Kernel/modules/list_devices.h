//
// Device list command module

void command_device(void);

struct ModuleDeviceCommand {
	
	ModuleDeviceCommand() {
		
		const char command_name[] = "device";
		kernel.function.install(&command_device, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleDeviceCommand moduleDeviceList;

void command_device(void) {
	
	string memoryFree(string_memory_allocator, sizeof(string_memory_allocator));
	
	kernel.console.consoleAddString(memoryFree, memoryFree.size());
	kernel.console.consoleNewLine();
	
	return;
}


