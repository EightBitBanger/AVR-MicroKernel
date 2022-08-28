#ifndef _RM_FUNCTION__
#define _RM_FUNCTION__

void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

char msg_file_not_found[] = "File not found.";
char msg_file_deleted[] = "File deleted.";

#define __MODULE_NAME_  "rm"

struct ModuleLoaderRm {
	ModuleLoaderRm() {
		load_device(__MODULE_NAME_, sizeof(__MODULE_NAME_), (Module)command_rm, DEVICE_TYPE_MODULE);
	}
}static moduleLoaderRm;


void command_rm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	char filename[11] = "          ";
	for (uint8_t a=0; a < 10; a++) 
		filename[a] = console.keyboard_string[sizeof(__MODULE_NAME_) + a];
	
	if (fs.file_delete(filename) == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
	} else {
		console.print(msg_file_deleted, sizeof(msg_file_deleted));
		console.printLn();
	}
	
	return;
}


#undef __MODULE_NAME_


#endif

