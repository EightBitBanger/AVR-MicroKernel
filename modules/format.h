//
// File system - format

#define _CHUNK_SIZE__             20
#define _FILE_NAME_MAX_LENGTH__   8
#define _PARTITION_SZ__           1000

void command_format(void);

struct ModuleLoaderFormat {
	ModuleLoaderFormat() {
		loadModule(command_format, "format",  7);
	}
} static moduleLoaderFormat;


void command_format(void) {
	
	WrappedPointer pointer;
	pointer.address = 0;
	DriverEntryPoint storageDriverPtr;
	
	// Link to system storage
	if (getFuncAddress(_HARD_STORAGE__, sizeof(_HARD_STORAGE__), storageDriverPtr) == 0) return;
	
	
	for (uint32_t i=0; i < _PARTITION_SZ__; i += _CHUNK_SIZE__) {
		
		
		// Set file marker to null
		callExtern(storageDriverPtr, 0x01, (uint8_t&)"x");
		
		pointer.address += _CHUNK_SIZE__;
		callExtern(storageDriverPtr, 0x00, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		
	}
	
	char msg_format_complete[] = "Format complete.";
	console.print(msg_format_complete,sizeof(msg_format_complete));
	console.printLn();
	return;
}


