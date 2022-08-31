#ifndef _RM_FUNCTION__
#define _RM_FUNCTION__

void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

char msg_file_deleted[] = "File deleted.";

#define __MODULE_NAME_  "rm"

struct ModuleLoaderRm {
	ModuleLoaderRm() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_rm, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderRm;


void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	for (uint8_t a=0; a < 10; a++) 
		call_extern(storageDevice, a, (uint8_t&)console.keyboard_string[sizeof(__MODULE_NAME_) + a]);
	
	// Delete the file
	uint8_t return_byte;
	call_extern(storageDevice, 0x0b, return_byte);
	
	if (return_byte == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
	} else {
		console.print(msg_file_deleted, sizeof(msg_file_deleted));
		console.printLn();
	}
	
	uint8_t char_space=0x20;
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, char_space);
	
	return;
}


#undef __MODULE_NAME_


#endif

