#ifndef _RM_FUNCTION_
#define _RM_FUNCTION_

void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

char msg_file_not_found[] = "File not found.";

#define __MODULE_NAME_  "rm"

struct ModuleLoaderRm {
	
	uint32_t current_device;
	
	ModuleLoaderRm() {
		
		current_device = 0x40000;
		
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
	uint32_t device_start  = moduleLoaderRm.current_device + 32;
	uint32_t device_end    = moduleLoaderRm.current_device + (1024 * 8);
	
	uint8_t page_counter=0;
	
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
			
			if (byte == 0xff) continue; // Ignore file data sectors
			if (byte == 0xaa) continue; // Ignore file end sectors
			
			// Get files name
			char current_file_name[11] = "          ";
			for (uint32_t a=1; a < 10; a++) {
				pointer.address = i + a;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				call_extern(storageDevice, 0x00, (uint8_t&)current_file_name[a-1]);
			}
			
			// Compare filenames
			if (strcmp(current_file_name, filename, 10) == 1) {
				
				// Get file size
				WrappedPointer filesize;
				for (uint32_t a=0; a < 4; a++) {
					pointer.address = i + a + 11;
					call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
					call_extern(storageDevice, 0x00, (uint8_t&)filesize.byte_t[a]);
				}
				
				uint32_t number_of_sectors = (filesize.address / 32) + 1;
				
				// Zero the file sectors
				byte = 0x00;
				for (uint32_t a=0; a < number_of_sectors; a++) {
					
					pointer.address = i + (a * stride);
					call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
					call_extern(storageDevice, 0x01, (uint8_t&)byte);
					_delay_ms(5);
				}
				
				return;
			}
			
		}
		
		
	}
	
	console.print(msg_file_not_found, sizeof(msg_file_not_found));
	console.printLn();
	
	return;
}


#undef __MODULE_NAME_


#endif

