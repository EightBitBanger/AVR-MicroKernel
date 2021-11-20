//
// Clear screen command module
namespace Modules {
struct ModuleClearScreen {
	
	ModuleClearScreen() {
		
		// Command name
		const char name[] = "cls";
		
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
	
};
static ModuleClearScreen moduleClearScreen;
}


