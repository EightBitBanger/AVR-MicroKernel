//
// Application test bed

void application_task(void);


void application_entry_point(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	//task_create("app", sizeof("app"), application_task, TASK_PRIORITY_NORMAL, TASK_TYPE_USER);
	
	uint8_t last_char=0;
	
	while(1) {
		
		Bus device_bus;
		device_bus.waitstate_read  = 2;
		device_bus.waitstate_write = 0;
		
		char byteLow, byteHigh;
		bus_read_byte(device_bus, 0x90000, byteLow);
		bus_read_byte(device_bus, 0xa0000, byteHigh);
		
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

