//
// Application example

void application_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
void application_task(void);

#define __MODULE_NAME_  "app"

struct ModuleLoaderApplication {
	
	ModuleLoaderApplication() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (Device)application_entry_point, _DEVICE_TYPE_MODULE__);
	}
}static applicationModuleLoader;



void application_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), application_task, _TASK_PRIORITY_NORMAL__, _TASK_USER__);
	
	uint8_t last_char=0;
	
	while(1) {
		
		char byteLow, byteHigh;
		bus_read_byte(keyboard.device_bus, _KEYBOARD_REGISTER_LO__, byteLow);
		bus_read_byte(keyboard.device_bus, _KEYBOARD_REGISTER_HI__, byteHigh);
		
		uint8_t current_char = byteLow;
		
		if (current_char != last_char) {
			
			last_char = current_char;
			
			char hex_begin[] = "0x";
			console.print(hex_begin, sizeof(hex_begin));
			console.printHex(byteLow);
			console.printLn();
			
			console.print(hex_begin, sizeof(hex_begin));
			console.printHex(byteHigh);
			console.printLn();
			
			
		}
		
	}
	
	return;
}

void application_task(void) {
	
	return;
}

#undef __MODULE_NAME_

