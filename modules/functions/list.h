//
// List command

void command_list(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "list"

struct ModuleLoaderList {
	
	ModuleLoaderList() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_list, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderList;


void command_list(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1 = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	
	//if ((param0 < '1') | (param0 > '9')) return;
	
	WrappedPointer pointer;
	char byte;
	
	uint32_t device_address = 0x30000 + (0x10000 * (param0 - '0'));
	
	// List data
	uint8_t list_counter=0;
	for (uint32_t i=device_address; i < (device_address + 10); i++) {
		
		pointer.address = i;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDevice, 0x00, (uint8_t&)byte);
		
		if (param1 == 'h') {
			
			console.printHex(byte);
			
		} else {
			
			console.printChar(byte);
			
		}
		
	}
	
	
	if (param1 != 'h') {
		
		console.printLn();
		
	}
	
	return;
}


#undef __MODULE_NAME_

