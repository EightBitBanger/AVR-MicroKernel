//
// Audio test app

void __application_main_(void);

struct ModuleLoaderApplication {
	ModuleLoaderApplication() {
		load_module("beep",  5, __application_main_);
	}
}static moduleLoaderApplication;

void executable_task(void);

//
// Entry point

void __application_main_(void) {
	
	Bus device_bus;
	device_bus.waitstate_write = 0;
	
	uint32_t freq  = console.keyboard_string[5];
	uint8_t length = 300;
	uint8_t delta  = 0;
	
	if ((freq >= '0') && (freq <= '9')) {delta = freq - '0';}
	if ((freq >= 'a') && (freq <= 'z')) {delta = freq - 'a';}
	
	Device audioDriver;
	get_func_address("Speaker", sizeof("Speaker"), audioDriver);
	call_extern(audioDriver, 0x00, length, delta);
	
	return;
}


