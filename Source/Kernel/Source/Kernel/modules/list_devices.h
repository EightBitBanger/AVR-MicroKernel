//
// Device list command module
namespace ModuleDeviceList {

void command_function(void);

struct ModuleDeviceList {
	
	ModuleDeviceList() {
		
		// Command name
		char name[] = "device";
		
		// Find a free command function index for our function
		uint8_t index = kernel.getFreeFunctionIndex();
		
		// Install the function pointer
		kernel.installFunction(index, &command_function, name, sizeof(name));
		
		return;
	}
	
};
static ModuleDeviceList moduleDeviceList;

void command_function(void) {
	
	// Clear the buffer and mask
	displayDriver.clearBuffer();
	displayDriver.clearMask();
	kernel.clearKeyboardString();
	
	// Reset the cursor
	kernel.cursorLine  = 0;
	kernel.cursorPos   = 1;
	
	_delay_ms(100);
	
	// Initiate the prompt
	kernel.consoleNewPrompt();
	
	kernel.promptState = 1;
	
	return;
}

}

