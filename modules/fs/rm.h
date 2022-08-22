#ifndef _RM_FUNCTION_
#define _RM_FUNCTION_

void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "rm"

struct ModuleLoaderRm {
	
	ModuleLoaderRm() {
		
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_rm, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderRm;


void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof(__MODULE_NAME_)];
	
	WrappedPointer pointer;
	char byte;
	
	// List current device contents
	uint8_t  stride        = 32;
	uint32_t device_start  = moduleLoaderDir.current_device + 32;
	uint32_t device_end    = moduleLoaderDir.current_device + (1024 * 8);
	
	// Get selected name
	char filename[11] = "          ";
	for (uint8_t a=0; a < 10; a++) {
		filename[a] = console.keyboard_string[sizeof(__MODULE_NAME_) + a];
	}
	
	for (uint32_t i=device_start; i < device_end; i += stride) {
		
		pointer.address = i;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDevice, 0x00, (uint8_t&)byte);
		
		// Check sector header byte
		if (byte != 0x00) {
			
			// Get files name
			char current_file_name[11] = "          ";
			for (uint8_t a=1; a < 10; a++) {
				pointer.address = i + a;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				call_extern(storageDevice, 0x00, (uint8_t&)current_file_name[a-1]);
			}
			
			if (strcmp(current_file_name, filename, 10) == 1) {
				
				byte = 0x00;
				
				pointer.address = i;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				call_extern(storageDevice, 0x01, (uint8_t&)byte);
				
				return;
			}
			
		}
		
		
	}
	 
	char filenotfound[] = "File not found.";
	console.print(filenotfound, sizeof(filenotfound));
	console.printLn();
	
	return;
}


#undef __MODULE_NAME_


#endif

