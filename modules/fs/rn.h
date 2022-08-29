#ifndef _RN_FUNCTION__
#define _RN_FUNCTION__

void command_rn(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "rn"

struct ModuleLoaderRn {
	ModuleLoaderRn() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_rn, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderRn;


void command_rn(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	char filenameA[32]; // Source name
	char filenameB[32]; // New name
	
	for (uint8_t i=0; i < 32; i++) {
		filenameA[i] = 0x20;
		filenameB[i] = 0x20;
	}
	
	// Split keyboard string names
	uint8_t swtch=0;
	uint8_t i=0;
	for (uint8_t a=0; a < 32; a++) {
		
		char str_char = console.keyboard_string[sizeof(__MODULE_NAME_) + a];
		
		if ((str_char == 0x20) & (swtch == 0)) {
			swtch = 1;
			i=0;
			continue;
		}
		
		if (swtch == 0) {filenameA[i] = str_char; i++;}
		
		if (swtch == 1) {
			if (str_char == 0x20) break;
			filenameB[i] = str_char;
			i++;
		}
		
	}
	
	// Open the file to be renamed
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, (uint8_t&)filenameA[a]);
	uint8_t state;
	call_extern(storageDevice, 0x0d, state);
	
	// Check source file exists
	if (state == 0) 
		return;
	
	// Rename to the new file name
	for (uint8_t a=0; a < 10; a++) 
		call_extern(storageDevice, a, (uint8_t&)filenameB[a]);
	call_extern(storageDevice, 0x13);
	
	return;
}


#undef __MODULE_NAME_


#endif

