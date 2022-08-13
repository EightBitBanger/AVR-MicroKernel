//
// On-board PS/2 keyboard device driver

void decodeScanCode(uint8_t scancode_low, uint8_t scancode_high, uint8_t& scan_code);

void keyboardDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define  _KEYBOARD_REGISTER_LO__   0x90000 // Lower keyboard register address
#define  _KEYBOARD_REGISTER_HI__   0xa0000 // Upper keyboard register address


struct KeyboardDriver {
	
	Bus device_bus;
	
	KeyboardDriver() {
		
		device_bus.waitstate_read  = 0;
		device_bus.waitstate_write = 0;
		
		load_device(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), (void(*)())keyboardDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
	}
	
	void initiate(void) {}
	
	void shutdown(void) {}
	
	// Read the current keyboard character
	void read(uint8_t& scan_code) {
		
		char byteLow, byteHigh;
		bus_read_byte(device_bus, _KEYBOARD_REGISTER_LO__, (char&)byteLow);
		bus_read_byte(device_bus, _KEYBOARD_REGISTER_HI__, (char&)byteHigh);
		
		decodeScanCode(byteLow, byteHigh, scan_code);
		return;
	}
	
	// Read keyboard scan codes
	void read_scancodes(uint8_t& scan_code_low, uint8_t& scan_code_high) {
		
		char byteLow, byteHigh;
		bus_read_byte(device_bus, _KEYBOARD_REGISTER_LO__, (char&)scan_code_low);
		bus_read_byte(device_bus, _KEYBOARD_REGISTER_HI__, (char&)scan_code_high);
		
		return;
	}
	
}static keyboard;


void keyboardDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	if (functionCall == DEVICE_CALL_INITIATE) {keyboard.initiate(); return;}
	if (functionCall == DEVICE_CALL_SHUTDOWN) {keyboard.shutdown(); return;}
	
	if (functionCall == 0x00) {keyboard.read(paramA); return;}
	if (functionCall == 0x01) {decodeScanCode(paramA, paramB, paramC); return;}
	if (functionCall == 0x02) {keyboard.read_scancodes(paramA, paramB); return;}
	
	return;
}


// Decode the scan code returning an ASCII character
void decodeScanCode(uint8_t scancode_low, uint8_t scancode_high, uint8_t& scan_code) {
	
	scan_code = 0x00;
	
	if (scancode_low == 0xdf) {
		if (scancode_high == 0x9a) {scan_code = 0x05; return;} // Left
		if (scancode_high == 0x90) {scan_code = 'i'; return;}
		if (scancode_high == 0xc6) {scan_code = 's'; return;}
		if (scancode_high == 0x88) {scan_code = 'd'; return;}
		if (scancode_high == 0xca) {scan_code = 'f'; return;}
		if (scancode_high == 0xcc) {scan_code = 'h'; return;}
		if (scancode_high == 0x8e) {scan_code = 'j'; return;}
		if (scancode_high == 0xd2) {scan_code = 'l'; return;}
		return;
	}
	
	if (scancode_low == 0x9f) {
		if (scancode_high == 0xd9) {scan_code = 0x01; return;} // Backspace
		if (scancode_high == 0xd6) {scan_code = 0x02; return;} // Enter
		if (scancode_high == 0xdc) {scan_code = 0x04; return;} // Down
		if (scancode_high == 0x9d) {scan_code = 0x07; return;} // Escape
		if (scancode_high == 0x91) {scan_code = '9'; return;}
		if (scancode_high == 0x8f) {scan_code = '8'; return;}
		if (scancode_high == 0xcd) {scan_code = '6'; return;}
		if (scancode_high == 0xcb) {scan_code = '5'; return;}
		if (scancode_high == 0x89) {scan_code = '3'; return;}
		if (scancode_high == 0xc7) {scan_code = '2'; return;}
		if (scancode_high == 0x85) {scan_code = '1'; return;}
		if (scancode_high == 0x86) {scan_code = 'z'; return;}
		if (scancode_high == 0xc8) {scan_code = 'x'; return;}
		if (scancode_high == 0x8a) {scan_code = 'v'; return;}
		if (scancode_high == 0x8c) {scan_code = 'b'; return;}
		if (scancode_high == 0xce) {scan_code = 'm'; return;}
		if (scancode_high == 0xd0) {scan_code = 'k'; return;}
		return;
	}
	
	if (scancode_low == 0x5f) {
		if (scancode_high == 0x9d) {scan_code = 0x03; return;} // Up
		if (scancode_high == 0x8a) {scan_code = 0x20; return;} // Space
		if (scancode_high == 0xc4) {scan_code = 0x09; return;} // Alt
		if (scancode_high == 0xdc) {scan_code = 0x10; return;} // Delete
		if (scancode_high == 0x91) {scan_code = '0'; return;}
		if (scancode_high == 0x8f) {scan_code = '7'; return;}
		if (scancode_high == 0x89) {scan_code = '4'; return;}
		if (scancode_high == 0xc8) {scan_code = 'c'; return;}
		if (scancode_high == 0x8c) {scan_code = 'n'; return;}
		if (scancode_high == 0x85) {scan_code = 'q'; return;}
		if (scancode_high == 0xc7) {scan_code = 'w'; return;}
		if (scancode_high == 0xcb) {scan_code = 'r'; return;}
		if (scancode_high == 0xcd) {scan_code = 'y'; return;}
		if (scancode_high == 0xd3) {scan_code = 'p'; return;}
		return;
	}
	
	if (scancode_low == 0x1f) {
		if (scancode_high == 0xdd) {scan_code = 0x06; return;} // Right
		if (scancode_high == 0xc5) {scan_code = 0x08; return;} // Control
		if (scancode_high == 0xc9) {scan_code = 'e'; return;}
		if (scancode_high == 0x8b) {scan_code = 't'; return;}
		if (scancode_high == 0xcf) {scan_code = 'u'; return;}
		if (scancode_high == 0xd1) {scan_code = 'o'; return;}
		if (scancode_high == 0x87) {scan_code = 'a'; return;}
		if (scancode_high == 0x8d) {scan_code = 'g'; return;}
		return;
	}
	return;
}


