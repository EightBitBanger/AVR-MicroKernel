//
// Keyboard interface

// Keyboard key decode index array
const char scancodeIndex[] = {
	0x05, 'i', 's', 'd', 'f', 'h', 'j' ,'l',
	0x01, 0x02, 0x04, '9', '8', '6', '5', '3', '2', '1', 'z', 'x', 'v', 'b', 'm', 'k',
	0x03, 0x20, '0', '7', '4', 'c', 'n', 'q', 'w', 'r', 'y', 'p',
	0x06, 'e', 't', 'u', 'o', 'a', 'g'
};

struct KeyboardDriver {
	
	// Read the current keyboard scan code
	uint8_t read(void) {
		
		char byteLow, byteHigh;
		_BUS_UPPER_OUT__ = 0x00;
		memory_read(0x90000, byteLow);
		_BUS_UPPER_OUT__ = 0x00;
		memory_read(0xa0000, byteHigh);
		
		return decodeScanCode(byteLow, byteHigh);
	}
	
	// Decode the scan code returning an ASCII character
	uint8_t decodeScanCode(uint8_t scancode_low, uint8_t scancode_high) {
		
		/*
		// Display a keyboard scan code
		if (scancode_low & (1 << 7)) {displayDriver.writeChar(0x31, 3, 0);} else {displayDriver.writeChar(0x30, 3, 0);}
		if (scancode_low & (1 << 6)) {displayDriver.writeChar(0x31, 3, 1);} else {displayDriver.writeChar(0x30, 3, 1);}
		if (scancode_low & (1 << 5)) {displayDriver.writeChar(0x31, 3, 2);} else {displayDriver.writeChar(0x30, 3, 2);}
		if (scancode_low & (1 << 4)) {displayDriver.writeChar(0x31, 3, 3);} else {displayDriver.writeChar(0x30, 3, 3);}
		if (scancode_low & (1 << 3)) {displayDriver.writeChar(0x31, 3, 4);} else {displayDriver.writeChar(0x30, 3, 4);}
		if (scancode_low & (1 << 2)) {displayDriver.writeChar(0x31, 3, 5);} else {displayDriver.writeChar(0x30, 3, 5);}
		if (scancode_low & (1 << 1)) {displayDriver.writeChar(0x31, 3, 6);} else {displayDriver.writeChar(0x30, 3, 6);}
		if (scancode_low & (1 << 0)) {displayDriver.writeChar(0x31, 3, 7);} else {displayDriver.writeChar(0x30, 3, 7);}
		
		if (scancode_high & (1 << 7)) {displayDriver.writeChar(0x31, 3, 12);} else {displayDriver.writeChar(0x30, 3, 12);}
		if (scancode_high & (1 << 6)) {displayDriver.writeChar(0x31, 3, 13);} else {displayDriver.writeChar(0x30, 3, 13);}
		if (scancode_high & (1 << 5)) {displayDriver.writeChar(0x31, 3, 14);} else {displayDriver.writeChar(0x30, 3, 14);}
		if (scancode_high & (1 << 4)) {displayDriver.writeChar(0x31, 3, 15);} else {displayDriver.writeChar(0x30, 3, 15);}
		if (scancode_high & (1 << 3)) {displayDriver.writeChar(0x31, 3, 16);} else {displayDriver.writeChar(0x30, 3, 16);}
		if (scancode_high & (1 << 2)) {displayDriver.writeChar(0x31, 3, 17);} else {displayDriver.writeChar(0x30, 3, 17);}
		if (scancode_high & (1 << 1)) {displayDriver.writeChar(0x31, 3, 18);} else {displayDriver.writeChar(0x30, 3, 18);}
		if (scancode_high & (1 << 0)) {displayDriver.writeChar(0x31, 3, 19);} else {displayDriver.writeChar(0x30, 3, 19);}
		*/
		
		switch (scancode_low) {
			
			case 0xdf: {
				switch (scancode_high) {
					case 0x9a: return scancodeIndex[0];
					case 0x90: return scancodeIndex[1];
					case 0xc6: return scancodeIndex[2];
					case 0x88: return scancodeIndex[3];
					case 0xca: return scancodeIndex[4];
					case 0xcc: return scancodeIndex[5];
					case 0x8e: return scancodeIndex[6];
					case 0xd2: return scancodeIndex[7];
					default: return 0x00;
				}
			}
			
			case 0x9f: {
				switch (scancode_high) {
					case 0xd9: return scancodeIndex[8];
					case 0xd6: return scancodeIndex[9];
					case 0xdc: return scancodeIndex[10];
					case 0x91: return scancodeIndex[11];
					case 0x8f: return scancodeIndex[12];
					case 0xcd: return scancodeIndex[13];
					case 0xcb: return scancodeIndex[14];
					case 0x89: return scancodeIndex[15];
					case 0xc7: return scancodeIndex[16];
					case 0x85: return scancodeIndex[17];
					case 0x86: return scancodeIndex[18];
					case 0xc8: return scancodeIndex[19];
					case 0x8a: return scancodeIndex[20];
					case 0x8c: return scancodeIndex[21];
					case 0xce: return scancodeIndex[22];
					case 0xd0: return scancodeIndex[23];
					default: return 0x00;
				}
			}
			
			case 0x5f: {
				switch (scancode_high) {
					case 0x9d: return scancodeIndex[24];
					case 0x8a: return scancodeIndex[25];
					case 0x91: return scancodeIndex[26];
					case 0x8f: return scancodeIndex[27];
					case 0x89: return scancodeIndex[28];
					case 0xc8: return scancodeIndex[29];
					case 0x8c: return scancodeIndex[30];
					case 0x85: return scancodeIndex[31];
					case 0xc7: return scancodeIndex[32];
					case 0xcb: return scancodeIndex[33];
					case 0xcd: return scancodeIndex[34];
					case 0xd3: return scancodeIndex[35];
					default: return 0x00;
				}
			}
			
			case 0x1f: {
				switch (scancode_high) {
					case 0xdd: return scancodeIndex[36];
					case 0xc9: return scancodeIndex[37];
					case 0x8b: return scancodeIndex[38];
					case 0xcf: return scancodeIndex[39];
					case 0xd1: return scancodeIndex[40];
					case 0x87: return scancodeIndex[41];
					case 0x8d: return scancodeIndex[42];
					default: return 0x00;
				}
			}
			
			default: return 0x00;
		}
		
	}
	
};
KeyboardDriver keyboard;


