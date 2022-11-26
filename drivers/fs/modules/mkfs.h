//
// Storage format function

char in_prog_str[] = "Formatting device...";

void command_fdisk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ModuleLoaderFormat {
	
	uint8_t format_size;
	
	ModuleLoaderFormat() {
		
		format_size = 8;
		
	}
	
}static moduleLoaderFormat;


void command_make_fs(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) 
		return;
	
	// Check the current device
	uint32_t current_device = fs_set_device_scope();
	
	uint8_t param0 = console.keyboard_string[sizeof("mkfs")];
	uint8_t param1 = console.keyboard_string[sizeof("mkfs") + 1];
	
	uint8_t param2 = console.keyboard_string[sizeof("mkfs") + 3];
	uint8_t param3 = console.keyboard_string[sizeof("mkfs") + 4];
	
	// Set the format size
	if ((param0 == '-') & (param1 == 's')) {
		
		if ((param2 > '0') & (param2 <= '9')) moduleLoaderFormat.format_size = param2 - '0';
		if (param2 == 'a') moduleLoaderFormat.format_size = 16;
		if (param2 == 'b') moduleLoaderFormat.format_size = 32;
		
		char in_prog_str[] = "Capacity ";
		console.print(in_prog_str, sizeof(in_prog_str));
		console.printInt(moduleLoaderFormat.format_size * 1024);
		console.printLn();
		
		return;
	}
	
	// Format a target device
	if ((param0 == '-') & (param1 == 'f')) {
		
		WrappedPointer pointer;
		uint8_t write_counter = 0;
		
		write_counter=0;
		
		if (((param2 < 'a') | (param2 > 'e')) & param2 != '/') {
			console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
			console.printLn();
			return;
		}
		
		console.print(in_prog_str, sizeof(in_prog_str));
		console.printLn();
		char current_device_letter = console.promptString[0];
		console.promptString[0] = param2;
		
		if (param2 == '/') {
			current_device = _VIRTUAL_STORAGE_ADDRESS__;
		} else {
			current_device = 0x40000 + (0x10000 * (param2 - 'a'));
		}
		
		fs_device_format(current_device, (moduleLoaderFormat.format_size * 1024));
		
		console.promptString[0] = current_device_letter;
		return;
	}
	
	return;
}


