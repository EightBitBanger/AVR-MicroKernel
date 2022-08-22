//
// Application example

void application_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);
void application_task(void);

#define __MODULE_NAME_  "app"

struct ModuleLoaderApplication {
	
	ModuleLoaderApplication() {
		load_device(__MODULE_NAME_,  sizeof(__MODULE_NAME_), (void(*)())application_entry_point, DEVICE_TYPE_MODULE);
	}
}static applicationModuleLoader;



void application_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	task_create(__MODULE_NAME_, sizeof(__MODULE_NAME_), application_task, TASK_PRIORITY_NORMAL, TASK_TYPE_USER);
	
	
	
	/*
	
	uint8_t last_char=0;
	
	while(1) {
		
		
		continue;
		
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
	*/
	
	return;
}

uint32_t second_counter=0;

void application_task(void) {
	
	if (timer_ms > 1000) {
		
		timer_ms = 0;
		second_counter++;
		
		uint8_t line = console.cursorLine;
		uint8_t pos  = console.cursorPos;
		
		console.setCursorPosition(0, 10);
		console.printInt(second_counter);
		
		console.setCursorPosition(line, pos);
		console.updateCursorPosition();
		
	}
	
	return;
}

#undef __MODULE_NAME_

