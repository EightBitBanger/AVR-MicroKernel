#ifndef _DIR_COMMAND__
#define _DIR_COMMAND__

void command_dir(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "dir"

struct ModuleLoaderDir {
	
	uint32_t current_device;
	
	char    current_directory[8];
	
	ModuleLoaderDir() {
		
		current_device = 0x40000;
		
		for (uint8_t i=0; i < 8; i++) 
			current_directory[i]=' ';
		
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_dir, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderDir;


void command_dir(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	// Link to the storage driver
	Device storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	uint8_t param0 = console.keyboard_string[sizeof(__MODULE_NAME_)];
	uint8_t param1 = console.keyboard_string[sizeof(__MODULE_NAME_) + 2];
	
	
	WrappedPointer pointer;
	uint8_t page_counter=0;
	char byte;
	
	// List current device contents
	uint8_t  stride        = 32;
	uint32_t device_start  = moduleLoaderDir.current_device + stride;
	uint32_t device_end    = moduleLoaderDir.current_device + (1024 * 8);
	
	
	for (uint32_t i=device_start; i < device_end; i += stride) {
		
		pointer.address = i;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		call_extern(storageDevice, 0x00, (uint8_t&)byte);
		
		if (byte != 0x00) {
			
			if (byte == 0xff) continue; // Ignore file data sectors
			if (byte == 0xaa) continue; // Ignore file end sectors
			
			// Display filename
			for (uint8_t a=0; a < 10; a++) {
				pointer.address = i + a + 1;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				call_extern(storageDevice, 0x00, (uint8_t&)byte);
				console.printChar(byte);
			}
			
			// Display file size
			WrappedPointer filesize;
			for (uint8_t a=0; a < 4; a++) {
				
				pointer.address = i + a + 11;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				call_extern(storageDevice, 0x00, (uint8_t&)filesize.byte_t[a]);
			}
			
			console.printSpace();
			console.printInt(filesize.address);
			
			page_counter++;
			console.printLn();
		}
		
		// Page pause
		if (page_counter > 2) {page_counter = 0; console.pause_press_anykey();}
		
	}
	
	return;
}


#undef __MODULE_NAME_








#endif

