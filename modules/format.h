//
// File system - format

void command_format(void);

struct ModuleLoaderFormat {
	ModuleLoaderFormat() {
		loadModule(command_format, "format",  7);
	}
} static moduleLoaderFormat;


void command_format(void) {
	
	return;
}


