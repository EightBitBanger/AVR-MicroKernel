//
// Device list command module

void command_device(void);

struct ModuleDeviceCommand {
	
	ModuleDeviceCommand() {
		
		const char command_name[] = "device";
		kernel.installFunction(&command_device, command_name, sizeof(command_name));
		
		return;
	}
	
};
static ModuleDeviceCommand moduleDeviceList;

void command_device(void) {
	
	string memoryFree(string_memory_allocator, sizeof(string_memory_allocator));
	
	kernel.consoleAddString(memoryFree, memoryFree.size());
	kernel.consoleNewLine();
	
	return;
}


