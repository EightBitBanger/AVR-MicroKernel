#ifndef _ASM_FUNCTION__
#define _ASM_FUNCTION__

void command_asm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ModuleLoaderAsm {
	
	uint32_t page_offset;
	uint32_t write_offset;
	
	ModuleLoaderAsm() {
		
		page_offset = 0;
		write_offset = 0;
		
	}
}static moduleLoaderAsm;

void command_asm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&) {
	
	Device storageDevice;
	storageDevice = (Device)get_func_address(_MASS_STORAGE__, sizeof(_MASS_STORAGE__));
	if (storageDevice == 0) return;
	
	char filename[10];
	for (uint8_t i=0; i < 10; i++) 
		filename[i] = 0x20;
	
	// Get file name from keyboard string
	for (uint8_t a=0; a < 10; a++) 
		filename[a] = console.keyboard_string[sizeof("asm") + a];
	
	
	
	
	// Write a HEX byte to the file
	if ((filename[0] == 'w') & (filename[1] == ' ') & (filename[2] != ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("asm") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("asm") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			char byte = string_get_hex_char(hex_string);
			uint32_t file_offset = moduleLoaderAsm.page_offset;
			file_write_byte(file_offset, byte);
			
			moduleLoaderAsm.page_offset++;
		}
		
		// Allow time after page write before an immediate read
		_delay_ms(5);
		
		console.printHex( moduleLoaderAsm.write_offset );
		console.printSpace();
		
		for (uint8_t i=0; i < 8; i++) {
			
			if (i == 4) console.printChar('-');
			
			char byte;
			uint32_t file_offset = (i) + moduleLoaderAsm.write_offset;
			file_read_byte(file_offset, byte);
			
			console.printHex( byte );
		}
		
		return;
	}
	
	
	// Set the write pointer
	if ((filename[0] == 'p') & (filename[1] == ' ') & (filename[2] != ' ')) {
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("asm") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("asm") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			moduleLoaderAsm.page_offset = string_get_hex_char(hex_string);
			moduleLoaderAsm.write_offset = moduleLoaderAsm.page_offset;
			
			console.printHex( moduleLoaderAsm.page_offset );
			console.printSpace();
			
			for (uint8_t i=0; i < 8; i++) {
				
				if (i == 4) console.printChar('-');
				
				char byte;
				uint32_t file_offset = (i) + moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				
				console.printHex( byte );
			}
			
		}
		
		return;
	}
	
	// Dump the current HEX page
	if ((filename[0] == 'd') & (filename[1] == ' ')) {
		
		char byte;
		uint8_t hex_value = 0x00;
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("asm") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("asm") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			hex_value = string_get_hex_char(hex_string);
			
			moduleLoaderAsm.page_offset = hex_value;
			
		}
		
		for (uint8_t i=0; i < 3; i++) {
			
			console.printHex( moduleLoaderAsm.page_offset );
			
			console.printSpace();
			
			for (uint8_t a=0; a < 8; a++) {
				
				if (a == 4) console.printChar('-');
				
				uint32_t file_offset = (a + moduleLoaderAsm.page_offset);
				
				file_read_byte(file_offset, byte);
				
				console.printHex(byte);
			}
			
			//console.printLn();
			
			moduleLoaderAsm.page_offset += 8;
		}
		
		return;
	}
	
	
	
	
	// Unassemble the current page
	if ((filename[0] == 'u') & (filename[1] == ' ')) {
		
		char byte;
		uint8_t hex_value = 0x00;
		
		char hex_string[2] = {'0', '0'};
		uint8_t char_b = console.keyboard_string[sizeof("asm") + 2];
		uint8_t char_a = console.keyboard_string[sizeof("asm") + 3];
		
		if ((((char_a >= '0') & (char_a <= '9')) | ((char_a >= 'a') & (char_a <= 'f'))) |
		(((char_b >= '0') & (char_b <= '9')) | ((char_b >= 'a') & (char_b <= 'f')))) {
			
			hex_string[0] = char_a;
			hex_string[1] = char_b;
			
			hex_value = string_get_hex_char(hex_string);
			
			moduleLoaderAsm.page_offset = hex_value;
			
		}
		
		// Check next few bytes for an opcode
		uint8_t asm_counter=0;
		byte = 0x00;
		uint16_t i=0;
		
		while (i<0xffff) {i++;
			
			uint8_t file_offset = moduleLoaderAsm.page_offset;
			file_read_byte(file_offset, byte);
			
			if (moduleLoaderAsm.page_offset > 0xfd) {
				moduleLoaderAsm.page_offset == 0xff;
				console.printHex(0xff);
				console.printSpace();
				console.print("end", sizeof("end"));
				
				console.printLn();
				break;
			}
			
			moduleLoaderAsm.page_offset++;
			
			// ADD  0x01
			if (byte == 0x01) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("add ", sizeof("add "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				if (byte == 0x00) console.print("ax", sizeof("  "));
				if (byte == 0x01) console.print("bx", sizeof("  "));
				if (byte == 0x02) console.print("cx", sizeof("  "));
				if (byte == 0x03) console.print("dx", sizeof("  "));
				console.printSpace();
				
				moduleLoaderAsm.page_offset++;
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				if (byte == 0x00) console.print("ax", sizeof("  "));
				if (byte == 0x01) console.print("bx", sizeof("  "));
				if (byte == 0x02) console.print("cx", sizeof("  "));
				if (byte == 0x03) console.print("dx", sizeof("  "));
				console.printSpace();
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			// ADD  0x02
			if (byte == 0x02) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("add ", sizeof("add "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				if (byte == 0x00) console.print("ax", sizeof("  "));
				if (byte == 0x01) console.print("bx", sizeof("  "));
				if (byte == 0x02) console.print("cx", sizeof("  "));
				if (byte == 0x03) console.print("dx", sizeof("  "));
				console.printSpace();
				
				moduleLoaderAsm.page_offset++;
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			// MOV  0xa0
			if (byte == 0xa0) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("mov ", sizeof("mov "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				if (byte == 0x00) console.print("ax", sizeof("  "));
				if (byte == 0x01) console.print("bx", sizeof("  "));
				if (byte == 0x02) console.print("cx", sizeof("  "));
				if (byte == 0x03) console.print("dx", sizeof("  "));
				console.printSpace();
				
				moduleLoaderAsm.page_offset++;
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			// MOVr  0xa1
			if (byte == 0xa1) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("mov ", sizeof("mov "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				if (byte == 0x00) console.print("ax", sizeof("  "));
				if (byte == 0x01) console.print("bx", sizeof("  "));
				if (byte == 0x02) console.print("cx", sizeof("  "));
				if (byte == 0x03) console.print("dx", sizeof("  "));
				console.printSpace();
				
				moduleLoaderAsm.page_offset++;
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				if (byte == 0x00) console.print("ax", sizeof("  "));
				if (byte == 0x01) console.print("bx", sizeof("  "));
				if (byte == 0x02) console.print("cx", sizeof("  "));
				if (byte == 0x03) console.print("dx", sizeof("  "));
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			// INT 0x3d | 0xcd
			if ((byte == 0xcd) | (byte == 0x3d)) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("int ", sizeof("int "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			// JMP 0xea
			if (byte == 0xea) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("jmp ", sizeof("jmp "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			
			// PUSH 0x06
			if (byte == 0x06) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("push ", sizeof("push "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			
			// POP 0x07
			if (byte == 0x07) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("pop ", sizeof("pop "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			
			// CALL 0x9a
			if (byte == 0x9a) {
				
				console.printHex( moduleLoaderAsm.page_offset - 1 );
				console.printSpace();
				
				console.print("call ", sizeof("call "));
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				console.printSpace();
				
				moduleLoaderAsm.page_offset++;
				
				file_offset = moduleLoaderAsm.page_offset;
				file_read_byte(file_offset, byte);
				console.printHex(byte);
				
				moduleLoaderAsm.page_offset++;
				
				console.printLn();
				asm_counter++;
			}
			
			// Display only 3 lines at a time
			if (asm_counter > 2) 
				break;
			
		}
		
		return;
	}
	
	
	
	// Open file
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, (uint8_t&)filename[a]);
	
	uint8_t return_value;
	call_extern(storageDevice, 0x0d, return_value);
	
	uint8_t blank_byte = 0x20;
	for (uint8_t a=0; a < 10; a++)
		call_extern(storageDevice, a, blank_byte);
	
	if (return_value != 0) {
		console.print("Loaded.", sizeof("Loaded."));
		console.printLn();
	} else {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
	}
	
	return;
}

#endif

