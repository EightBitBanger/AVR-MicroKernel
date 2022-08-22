#ifndef _MK_FUNCTION_
#define _MK_FUNCTION_

void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "mk"

struct ModuleLoaderMk {
	
	ModuleLoaderMk() {
		
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_mk, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderMk;


void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1 = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	uint8_t param2 = console.keyboard_string[sizeof(__MODULE_NAME_) + 3];
	
	WrappedPointer pointer;
	char byte;
	
	uint8_t  stride        = 32;
	uint32_t device_start  = moduleLoaderDir.current_device + 32;
	uint32_t device_end    = moduleLoaderDir.current_device + (32 * 256);
	
	// Get filename parameter
	char filename[11] = "          ";
	for (uint8_t a=0; a < 10; a++) {
		uint8_t str_char = console.keyboard_string[sizeof(__MODULE_NAME_) + a];
		if (str_char == ' ') break;
		filename[a] = str_char;
	}
	
	for (uint32_t i=device_start; i < device_end; i += stride) {
		
		pointer.address = i;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDevice, 0x00, (uint8_t&)byte);
		
		// Check sector is empty
		if (byte == 0x00) {
			
			// Mark sector as used
			byte = 0xff;
			pointer.address = i;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			call_extern(storageDevice, 0x01, (uint8_t&)byte);
			
			// Write filename
			for (uint8_t a=0; a < 10; a++) {
				pointer.address = i + a + 1;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				call_extern(storageDevice, 0x01, (uint8_t&)filename[a]);
			}
			
			// Initiate filesize
			WrappedPointer filesize;
			filesize.address = 1;
			for (uint8_t a=0; a < 4; a++) {
				
				pointer.address = i + a + 11;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				call_extern(storageDevice, 0x01, (uint8_t&)filesize.byte_t[a]);
				
			}
			
			// Mark data sector as used
			byte = 0x55;
			
			pointer.address = i + 32;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			call_extern(storageDevice, 0x01, (uint8_t&)byte);
			
			return;
		}
		
	}
	
	return;
}


#undef __MODULE_NAME_


#endif

