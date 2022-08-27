#ifndef _COPY_FUNCTION__
#define _COPY_FUNCTION__

void command_copy(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define __MODULE_NAME_  "copy"

struct ModuleLoaderCopy {
	
	ModuleLoaderCopy() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_copy, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderCopy;


void command_copy(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	char filenameA[10];
	char filenameB[10];
	
	for (uint8_t i=0; i < 10; i++) {
		filenameA[i] = 0x20;
		filenameB[i] = 0x20;
	}
	
	uint8_t swtch=0;
	uint8_t i=0;
	for (uint8_t a=0; a < 10; a++) {
		
		char str_char = console.keyboard_string[sizeof(__MODULE_NAME_) + a];
		
		if (str_char == 0x20) {
			if (swtch == 0) {
				swtch = 1;
			}
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
	
	uint32_t file_size = fs.file_get_size(filenameA);
	if (file_size != 0) {
		
		fs.file_create(filenameB, file_size);
		
	}
	
	return;
}


#undef __MODULE_NAME_


#endif

