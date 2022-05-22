//
// Keyboard interface

// Keyboard key decode index array
const char scancodeIndex[] = {
	0x05, 'i', 's', 'd', 'f', 'h', 'j' ,'l',
	0x01, 0x02, 0x04, '9', '8', '6', '5', '3', '2', '1', 'z', 'x', 'v', 'b', 'm', 'k',
	0x03, 0x20, '0', '7', '4', 'c', 'n', 'q', 'w', 'r', 'y', 'p',
	0x06, 'e', 't', 'u', 'o', 'a', 'g'
};

void keyboardDeviceDriverEntryPoint(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct KeyboardDriver {
	
	Bus device_bus;
	
	KeyboardDriver() {loadLibrary(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), (DriverEntryPoint)keyboardDeviceDriverEntryPoint);}
	
	void initiate(void) {
		
		device_bus.waitStateRead  = 10;
		device_bus.waitStateWrite = 10;
		
	}
	
	void shutdown(void) {
		
	}
	
	// Read the current keyboard character
	void read(uint8_t& scan_code) {
		
		char byteLow, byteHigh;
		device_bus.read(0x90000, byteLow);
		device_bus.read(0xa0000, byteHigh);
		
		decodeScanCode(byteLow, byteHigh, scan_code);
		return;
	}
	
	// Decode the scan code returning an ASCII character
	void decodeScanCode(uint8_t scancode_low, uint8_t scancode_high, uint8_t& scan_code) {
		
		switch (scancode_low) {
			
			case 0xdf: {
				switch (scancode_high) {
					case 0x9a: {scan_code = scancodeIndex[0]; return;}
					case 0x90: {scan_code = scancodeIndex[1]; return;}
					case 0xc6: {scan_code = scancodeIndex[2]; return;}
					case 0x88: {scan_code = scancodeIndex[3]; return;}
					case 0xca: {scan_code = scancodeIndex[4]; return;}
					case 0xcc: {scan_code = scancodeIndex[5]; return;}
					case 0x8e: {scan_code = scancodeIndex[6]; return;}
					case 0xd2: {scan_code = scancodeIndex[7]; return;}
					default: break;
				}
				break;
			}
			
			case 0x9f: {
				switch (scancode_high) {
					case 0xd9: {scan_code = scancodeIndex[8]; return;}
					case 0xd6: {scan_code = scancodeIndex[9]; return;}
					case 0xdc: {scan_code = scancodeIndex[10]; return;}
					case 0x91: {scan_code = scancodeIndex[11]; return;}
					case 0x8f: {scan_code = scancodeIndex[12]; return;}
					case 0xcd: {scan_code = scancodeIndex[13]; return;}
					case 0xcb: {scan_code = scancodeIndex[14]; return;}
					case 0x89: {scan_code = scancodeIndex[15]; return;}
					case 0xc7: {scan_code = scancodeIndex[16]; return;}
					case 0x85: {scan_code = scancodeIndex[17]; return;}
					case 0x86: {scan_code = scancodeIndex[18]; return;}
					case 0xc8: {scan_code = scancodeIndex[19]; return;}
					case 0x8a: {scan_code = scancodeIndex[20]; return;}
					case 0x8c: {scan_code = scancodeIndex[21]; return;}
					case 0xce: {scan_code = scancodeIndex[22]; return;}
					case 0xd0: {scan_code = scancodeIndex[23]; return;}
					default: break;
				}
				break;
			}
			
			case 0x5f: {
				switch (scancode_high) {
					case 0x9d: {scan_code = scancodeIndex[24]; return;}
					case 0x8a: {scan_code = scancodeIndex[25]; return;}
					case 0x91: {scan_code = scancodeIndex[26]; return;}
					case 0x8f: {scan_code = scancodeIndex[27]; return;}
					case 0x89: {scan_code = scancodeIndex[28]; return;}
					case 0xc8: {scan_code = scancodeIndex[29]; return;}
					case 0x8c: {scan_code = scancodeIndex[30]; return;}
					case 0x85: {scan_code = scancodeIndex[31]; return;}
					case 0xc7: {scan_code = scancodeIndex[32]; return;}
					case 0xcb: {scan_code = scancodeIndex[33]; return;}
					case 0xcd: {scan_code = scancodeIndex[34]; return;}
					case 0xd3: {scan_code = scancodeIndex[35]; return;}
					default: break;
				}
				break;
			}
			
			case 0x1f: {
				switch (scancode_high) {
					case 0xdd: scan_code = scancodeIndex[36]; return;
					case 0xc9: scan_code = scancodeIndex[37]; return;
					case 0x8b: scan_code = scancodeIndex[38]; return;
					case 0xcf: scan_code = scancodeIndex[39]; return;
					case 0xd1: scan_code = scancodeIndex[40]; return;
					case 0x87: scan_code = scancodeIndex[41]; return;
					case 0x8d: scan_code = scancodeIndex[42]; return;
					default: break;
				}
				break;
			}
			
			default: break;
		}
		
		scan_code = 0x00;
		return;
	}
	
};
KeyboardDriver keyboard;

void keyboardDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case _DRIVER_INITIATE__: {keyboard.initiate(); break;}
		case _DRIVER_SHUTDOWN__: {keyboard.shutdown(); break;}
		
		case 0x00: {keyboard.read(paramA); break;}
		case 0x01: {keyboard.decodeScanCode(paramA, paramB, paramC); break;}
		
		default: break;
	}
	
	return;
}



