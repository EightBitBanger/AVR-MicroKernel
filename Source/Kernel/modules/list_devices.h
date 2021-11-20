//
// Device list command module
namespace Modules {

struct ModuleDeviceList {
	
	ModuleDeviceList() {
		
		// Command name
		const char name[] = "device";
		
		uint8_t index = kernel.getFreeFunctionIndex();
		if (index == 0) return; else index--;
		
		// Install the function pointer and command name
		kernel.functionState[index] = 0xff;
		kernel.command_function_ptr[index] = command_function;
		
		for (uint8_t i=0; i < sizeof(name); i++) {kernel.functionNameIndex[index][i] = name[i];}
		
		return;
	}
	
	static void command_function(void) {
		
		// Clear the buffer and mask
		displayDriver.clearBuffer();
		displayDriver.clearMask();
		
		return;
	}
	
};
static ModuleDeviceList moduleDeviceList;


}


