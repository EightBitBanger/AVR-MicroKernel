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
		
		load_device(_KEYBOARD_INPUT__, sizeof(_KEYBOARD_INPUT__), (Device)keyboardDeviceDriverEntryPoint, DEVICE_TYPE_DRIVER);
		
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
	
	// Read keyboard scan codes
	void read_scancodes(uint8_t& scan_code_low, uint8_t& scan_code_high) {
		
		char byteLow, byteHigh;
		bus_read_byte(device_bus, _KEYBOARD_REGISTER_LO__, (char&)scan_code_low);
		bus_read_byte(device_bus, _KEYBOARD_REGISTER_HI__, (char&)scan_code_high);
		
		return;
	}
	
}static keyboard;


void keyboardDeviceDriverEntryPoint(uint8_t functionCall, uint8_t& paramA, uint8_t& paramB, uint8_t& paramC, uint8_t& paramD) {
	
	switch(functionCall) {
		
		case DEVICE_CALL_INITIATE: {keyboard.initiate(); break;}
		case DEVICE_CALL_SHUTDOWN: {keyboard.shutdown(); break;}
		
		case 0x00: {keyboard.read(paramA); break;}
		case 0x01: {decodeScanCode(paramA, paramB, paramC); break;}
		case 0x02: {keyboard.read_scancodes(paramA, paramB); break;}
		
		default: break;
	}
	
	return;
}


// Decode the scan code returning an ASCII character
void decodeScanCode(uint8_t scancode_low, uint8_t scancode_high, uint8_t& scan_code) {
	
	scan_code = 0x00;
	return;
}


