#ifndef _COPY_FUNCTION__
#define _COPY_FUNCTION__

void command_copy(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "copy"

char msg_file_copied[] = "File copied.";
char msg_file_cant_copy[] = "File cannot be copied.";

struct ModuleLoaderCopy {
	
	ModuleLoaderCopy() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_copy, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderCopy;

void command_copy(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
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
	
	// Check file exits
	uint32_t file_size = fs.file_get_size(filenameA);
	if (file_size != 0) {
		
		uint32_t destination_file_size = fs.file_get_size(filenameB);
		
		if (destination_file_size != 0) {
			console.print(msg_file_cant_copy, sizeof(msg_file_cant_copy));
			console.printLn();
			return;
		}
		
		fs.file_create(filenameB, file_size);
		
		console.print(msg_file_copied, sizeof(msg_file_copied));
		console.printLn();
	}
	
	return;
}


#undef __MODULE_NAME_


#endif

