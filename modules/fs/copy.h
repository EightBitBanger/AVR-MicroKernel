#ifndef _COPY_FUNCTION__
#define _COPY_FUNCTION__

void command_copy(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "copy"

#define MAX_COPY_BUFFER  512

char msg_file_copied[] = "File copied.";
char msg_file_cant_copy[] = "File cannot be copied.";
char msg_file_too_large[] = "File too large.";

struct ModuleLoaderCopy {
	
	ModuleLoaderCopy() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_copy, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderCopy;

void command_copy(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	char filenameA[32]; // Source
	char filenameB[32]; // Destination
	
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
	
	
	// Get destination file size
	WrappedPointer size_pointer;
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, (uint8_t&)filenameB[a]);
	call_extern(storageDevice, 0x0c, size_pointer.byte_t[0], size_pointer.byte_t[1], size_pointer.byte_t[2], size_pointer.byte_t[3]);
	uint32_t destination_file_size = size_pointer.address;
	
	// Check if the file already exists
	if (destination_file_size != 0) {
		console.print(msg_file_exists, sizeof(msg_file_exists));
		console.printLn();
		return;
	}
	
	// Get source file size
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, (uint8_t&)filenameA[a]);
	call_extern(storageDevice, 0x0c, size_pointer.byte_t[0], size_pointer.byte_t[1], size_pointer.byte_t[2], size_pointer.byte_t[3]);
	uint32_t file_size = size_pointer.address;
	
	// Check if the source file does not exist
	if (file_size == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
		
	} else {
		
		// Check memory buffer size
		if (file_size > MAX_COPY_BUFFER) {
			console.print(msg_file_too_large, sizeof(msg_file_too_large));
			console.printLn();
			return;
		}
		
		// Copy buffer
		char buffer[MAX_COPY_BUFFER];
		
		// Set file name
		for (uint8_t a=0; a < 10; a++)
			call_extern(storageDevice, a, (uint8_t&)filenameA[a]);
		
		// Open the source file
		uint8_t return_value;
		call_extern(storageDevice, 0x0d, return_value);
		
		if (return_value != 0) {
			
			// Copy data to the buffer
			for (uint32_t i=0; i < file_size; i++) {
				
				WrappedPointer pointer;
				pointer.address = i + 32;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				
				call_extern(storageDevice, 0x0f, (uint8_t&)buffer[i]);
				
			}
			
		}
		
		// Set file name
		for (uint8_t a=0; a < 10; a++)
			call_extern(storageDevice, a, (uint8_t&)filenameB[a]);
		
		// Set file size
		WrappedPointer pointer;
		pointer.address = file_size;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		
		// Create the new file
		call_extern(storageDevice, 0x0a);
		
		// Open the destination file
		call_extern(storageDevice, 0x0d, return_value);
		
		if (return_value != 0) {
			
			// Paste the buffer data
			for (uint32_t i=0; i < file_size; i++) {
				
				WrappedPointer pointer;
				pointer.address = i + 32;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				
				call_extern(storageDevice, 0x10, (uint8_t&)buffer[i]);
				
			}
		}
		
		uint8_t blank_byte = 0x20;
		for (uint8_t a=0; a < 10; a++)
			call_extern(storageDevice, a, blank_byte);
		
		console.print(msg_file_copied, sizeof(msg_file_copied));
		console.printLn();
	}
	
	return;
}


#undef __MODULE_NAME_


#endif

