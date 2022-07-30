//
// On-board PS/2 keyboard device driver

void decodeScanCode(uint8_t scancode_low, uint8_t scancode_high, uint8_t& scan_code);

void keyboardDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

#define  _KEYBOARD_REGISTER_LO__   0x90000 // Lower keyboard register address
#define  _KEYBOARD_REGISTER_HI__   0xa0000 // Upper keyboard register address


struct KeyboardDriver {
	
	Bus device_bus;
	
	KeyboardDriver() {
		
		device_bus.waitstate_read  = 1;
		device_bus.waitstate_write = 0;
		
		load_device(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), (Device)keyboardDeviceDriverEntryPoint, _DEVICE_TYPE_DRIVER__);
		
	}
	
	void initiate(void) {}
	
	void shutdown(void) {}
	
	// Read the current keyboard character
	void read(uint8_t& scan_code) {
		
		char byteLow, byteHigh;
		bus_read_byte(device_bus, _KEYBOARD_REGISTER_LO__, byteLow);
		bus_read_byte(device_bus, _KEYBOARD_REGISTER_HI__, byteHigh);
		
		decodeScanCode(byteLow, byteHigh, scan_code);
		return;
	}
	
}static keyboard;


void keyboardDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DEVICE_INITIATE__: {keyboard.initiate(); break;}
		case _DEVICE_SHUTDOWN__: {keyboard.shutdown(); break;}
		
		case 0x00: {keyboard.read(paramA); break;}
		case 0x01: {decodeScanCode(paramA, paramB, paramC); break;}
		
		default: break;
	}
	
	return;
}


// Decode the scan code returning an ASCII character
void decodeScanCode(uint8_t scancode_low, uint8_t scancode_high, uint8_t& scan_code) {
	
	switch (scancode_low) {
		
		case 0xdf: {
			switch (scancode_high) {
				case 0x9a: {scan_code = 0x05; return;}
				case 0x90: {scan_code = 'i'; return;}
				case 0xc6: {scan_code = 's'; return;}
				case 0x88: {scan_code = 'd'; return;}
				case 0xca: {scan_code = 'f'; return;}
				case 0xcc: {scan_code = 'h'; return;}
				case 0x8e: {scan_code = 'j'; return;}
				case 0xd2: {scan_code = 'l'; return;}
				default: break;
			}
			break;
		}
		
		case 0x9f: {
			switch (scancode_high) {
				case 0xd9: {scan_code = 0x01; return;} // Backspace
				case 0xd6: {scan_code = 0x02; return;} // Enter
				case 0xdc: {scan_code = 0x04; return;}
				case 0x91: {scan_code = '9'; return;}
				case 0x8f: {scan_code = '8'; return;}
				case 0xcd: {scan_code = '6'; return;}
				case 0xcb: {scan_code = '5'; return;}
				case 0x89: {scan_code = '3'; return;}
				case 0xc7: {scan_code = '2'; return;}
				case 0x85: {scan_code = '1'; return;}
				case 0x86: {scan_code = 'z'; return;}
				case 0xc8: {scan_code = 'x'; return;}
				case 0x8a: {scan_code = 'v'; return;}
				case 0x8c: {scan_code = 'b'; return;}
				case 0xce: {scan_code = 'm'; return;}
				case 0xd0: {scan_code = 'k'; return;}
				default: break;
			}
			break;
		}
		
		case 0x5f: {
			switch (scancode_high) {
				case 0x9d: {scan_code = 0x03; return;}
				case 0x8a: {scan_code = 0x20; return;} // Space
				case 0x91: {scan_code = '0'; return;}
				case 0x8f: {scan_code = '7'; return;}
				case 0x89: {scan_code = '4'; return;}
				case 0xc8: {scan_code = 'c'; return;}
				case 0x8c: {scan_code = 'n'; return;}
				case 0x85: {scan_code = 'q'; return;}
				case 0xc7: {scan_code = 'w'; return;}
				case 0xcb: {scan_code = 'r'; return;}
				case 0xcd: {scan_code = 'y'; return;}
				case 0xd3: {scan_code = 'p'; return;}
				default: break;
			}
			break;
		}
		
		case 0x1f: {
			switch (scancode_high) {
				case 0xdd: scan_code = 0x06; return;
				case 0xc9: scan_code = 'e'; return;
				case 0x8b: scan_code = 't'; return;
				case 0xcf: scan_code = 'u'; return;
				case 0xd1: scan_code = 'o'; return;
				case 0x87: scan_code = 'a'; return;
				case 0x8d: scan_code = 'g'; return;
				default: break;
			}
			break;
		}
		
		default: break;
	}
	
	scan_code = 0x00;
	return;
}


