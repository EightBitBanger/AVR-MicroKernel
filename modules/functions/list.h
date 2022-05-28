// List contents

void command_list(void);

struct ModuleLoaderList {
	ModuleLoaderList() {
		load_module("list",  5, command_list);
	}
} static loadModuleList;


void command_list(void) {
	
	return;
}


