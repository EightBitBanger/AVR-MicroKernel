#ifndef _MK_FUNCTION_
#define _MK_FUNCTION_

void command_mk(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "mk"

struct ModuleLoaderMk {
	
	uint32_t file_size;
	
	ModuleLoaderMk() {
		
		file_size       = 1;
		
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
	
	uint32_t current_device = 0x30000 + (0x10000 * (console.promptString[0] - 'A' + 1));
	uint32_t stride         = 32;
	uint32_t device_start   = current_device;
	uint32_t device_end     = current_device + (stride * 256);   // 8k
	
	uint8_t page_counter=0;
	
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
		if (byte != 0x00) continue;
		
		// Check next bytes for file size
		uint16_t count_free_sectors=0;
		for (uint32_t a=device_start; a < device_end; a += stride) {
			
			pointer.address = i;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			call_extern(storageDevice, 0x00, (uint8_t&)byte);
			
			if (byte != 0x00) continue;
			
			
			
			
		}
		
		
		// Mark sector as used
		byte = 0x55; // File start byte
		pointer.address = i;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDevice, 0x01, (uint8_t&)byte);
		
		_delay_ms(5);
		
		// Write file name
		for (uint32_t a=0; a < 10; a++) {
			pointer.address = i + a + 1;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			call_extern(storageDevice, 0x01, (uint8_t&)filename[a]);
			_delay_ms(5);
		}
		
		// Write file size
		WrappedPointer filesize;
		filesize.address = moduleLoaderMk.file_size;
		if (filesize.address == 0) filesize.address = 1;
		for (uint32_t a=0; a < 4; a++) {
			
			pointer.address = i + a + 11;
			call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
			call_extern(storageDevice, 0x01, (uint8_t&)filesize.byte_t[a]);
			_delay_ms(5);
		}
		
		uint32_t number_of_sectors = (moduleLoaderMk.file_size / 32) + 2;
		
		// Mark following sectors
		byte = 0xff;
		for (uint32_t a=1; a < number_of_sectors; a++) {
			
			if (a == number_of_sectors-1) {byte = 0xaa;
				
				pointer.address = i + (a * stride);
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				call_extern(storageDevice, 0x01, (uint8_t&)byte);
				_delay_ms(5);
			}
			
			return;
		}
		
	}
	
	return;
}


#undef __MODULE_NAME_


#endif

