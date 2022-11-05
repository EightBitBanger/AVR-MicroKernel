#ifndef _COPY_FUNCTION__
#define _COPY_FUNCTION__

#define MAX_COPY_BUFFER  1024

char msg_file_cant_copy[] = "File cannot be copied.";
char msg_file_too_large[] = "File too large.";

void command_copy(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	char file_source[32]; // Source name
	char file_dest[32]; // New name
	
	for (uint8_t i=0; i < 32; i++) {
		file_source[i] = 0x20;
		file_dest[i] = 0x20;
	}
	
	// Split keyboard string names
	uint8_t swtch=0;
	uint8_t i=0;
	for (uint8_t a=0; a < _MAX_KEYBOARD_STRING_LENGTH__ - sizeof("copy"); a++) {
		
		char str_char = console.keyboard_string[sizeof("copy") + a];
		
		if ((str_char == 0x20) & (swtch == 0)) {
			swtch = 1;
			i=0;
			continue;
		}
		
		if (swtch == 0) {
			file_source[i] = str_char;
			i++;
		}
		
		if (swtch == 1) {
			if (str_char == 0x20) break;
			file_dest[i] = str_char;
			i++;
		}
		
	}
	
	// Check if the destination file already exists
	uint32_t destination_file_size = file_get_size(file_dest);
	
	if (destination_file_size != 0) {
		console.print(msg_file_already_exists, sizeof(msg_file_already_exists));
		console.printLn();
		return;
	}
	
	// Get source file size
	uint32_t file_size = file_get_size(file_source);
	
	// Get source file attributes
	uint8_t attribute[4];
	for (uint8_t i=0; i < 4; i++) 
		attribute[i] = file_get_attribute(file_source, i);
	
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
		
		// Allocate a copy buffer on the stack
		uint32_t buffer_ptr;
		buffer_ptr = exMem.stack_push(MAX_COPY_BUFFER);
		
		// Open the source file and copy data to the buffer
		if (file_open(file_source) != 0) {
			
			for (uint32_t i=0; i < file_size; i++) {
				
				char buffer_byte;
				file_read_byte(i, buffer_byte);
				
				exMem.write(buffer_ptr + i, buffer_byte);
				
			}
			
		}
		
		// Set file size
		WrappedPointer pointer;
		pointer.address = file_size;
		call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
		
		uint8_t old_prompt_char;
		uint8_t return_value=0;
		
		// Check destination device scope
		if (file_dest[1] == ':') {
			old_prompt_char = console.promptString[0];
			console.promptString[0] = file_dest[0];
			
			file_create(file_source, pointer.address, attribute);
			return_value = file_open(file_source);
		} else {
			file_create(file_dest, pointer.address, attribute);
			return_value = file_open(file_dest);
		}
		
		if (return_value != 0) {
			
			// Set the file attributes
			for (uint8_t i=0; i < 4; i++)
				file_set_attribute(file_source, attribute[i], i);
			
			// Paste the buffer data to the new file
			for (uint32_t i=0; i < file_size; i++) {
				
				WrappedPointer pointer;
				pointer.address = i;
				call_extern(storageDevice, DEVICE_CALL_ADDRESS, pointer.byte_t[0], pointer.byte_t[1], pointer.byte_t[2], pointer.byte_t[3]);
				
				char buffer_byte;
				exMem.read(buffer_ptr + i, buffer_byte);
				
				call_extern(storageDevice, 0x10, (uint8_t&)buffer_byte);
				
			}
		}
		
		// Reset current directory
		if (file_dest[1] == ':') {console.promptString[0] = old_prompt_char;}
		
		uint8_t blank_byte = 0x20;
		for (uint8_t a=0; a < 10; a++)
			call_extern(storageDevice, a, blank_byte);
		
		exMem.stack_pop(MAX_COPY_BUFFER);
		
	}
	
	
	return;
}

#endif

