#ifndef _COPY_FUNCTION__
#define _COPY_FUNCTION__

#define MAX_COPY_BUFFER  1024

char msg_file_cant_copy[] = "File cannot be copied.";
char msg_file_too_large[] = "File too large.";

void command_copy(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	// Check the volume header of the current device
	uint32_t current_device = set_device_scope();
	
	if (device_check_header(_MASS_STORAGE__, current_device) == 0) {
		console.print(msg_device_not_ready, sizeof(msg_device_not_ready));
		console.printLn();
		return;
	}
	
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
	
	uint8_t old_prompt_string = console.promptString[0];
	
	// Check if the source file exists
	if (file_open(file_source) == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
		return;
	}
	
	uint32_t src_address = fs.file_address;
	uint32_t src_size = fs.file_size;
	
	// Get source file attributes
	uint8_t attributes[4];
	for (uint8_t i=0; i < 4; i++)
		fs.read(fs.file_address + 15 + i, (char&)attributes[i]);
	
	// Check destination device
	uint8_t file_state=0;
	if (file_dest[1] == ':') {
		console.promptString[0] = file_dest[0];
		file_state = file_open(file_source);
		if (file_state == 0) file_create(file_source, src_size, attributes);
	} else {
		file_state = file_open(file_dest);
		if (file_state == 0) file_create(file_dest, src_size, attributes);
	}
	
	// Check if the destination file exists
	if (file_state == 1) {
		console.promptString[0] = old_prompt_string;
		console.print(msg_file_already_exists, sizeof(msg_file_already_exists));
		console.printLn();
		return;
	}
	uint32_t dest_address = fs.file_address;
	
	// Allocate copy buffer
	char buffer[src_size];
	
	// Copy source file data
	console.promptString[0] = old_prompt_string;
	file_open(file_source);
	for (uint32_t i=0; i < src_size; i++) 
		file_read_byte(i, buffer[i]);
	
	if (file_dest[1] == ':') {
		console.promptString[0] = file_dest[0];
		file_open(file_source);
	} else {
		console.promptString[0] = old_prompt_string;
		file_open(file_dest);
	}
	
	// Write data to the destination
	for (uint32_t i=0; i < src_size; i++) 
	    file_write_byte(i, buffer[i]);
	
	console.promptString[0] = old_prompt_string;
	return;
}

#endif