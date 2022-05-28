//
// File system - format

void command_format(void);

struct ModuleLoaderFormat {
	ModuleLoaderFormat() {
		load_module("format",  7, command_format);
	}
} static moduleLoaderFormat;


void command_format(void) {
	
	return;
}


