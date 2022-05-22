// List contents

void command_list(void);

struct ModuleLoaderList {
	ModuleLoaderList() {
		loadModule(command_list, "list",  5);
	}
} static loadModuleList;


void command_list(void) {
	
	WrappedPointer pointer;
	pointer.address = 0;
	DriverEntryPoint storageDriverPtr;
	
	// Link to system storage
	if (getFuncAddress(_HARD_STORAGE__, sizeof(_HARD_STORAGE__), storageDriverPtr) == 0) return;
	
	// Set the address
	callExtern(storageDriverPtr, 0x00, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
	
	for (uint8_t line=0; line < 8; line++) {
		
		for (uint8_t col=0; col < 20; col++) {
			
			uint8_t byte=0x00;
			
			// Read the byte
			callExtern(storageDriverPtr, 0x04, byte);
			
			console.printChar(byte);
			
			pointer.address++;
			
		}
		
		_delay_ms(500);
		console.printLn();
		
	}
	
	return;
}


