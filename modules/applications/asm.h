#ifndef _ASM_FUNCTION__
#define _ASM_FUNCTION__

void command_asm(uint8_t, uint8_t&, uint8_t&, uint8_t&, uint8_t&);

struct ModuleLoaderAsm {
	
	uint32_t page_offset;
	uint32_t write_offset;
	uint32_t asm_counter;
	
	ModuleLoaderAsm() {
		
		page_offset = 0;
		write_offset = 0;
		
	}
}static moduleLoaderAsm;

uint8_t asm_decode_instruction(char byte);

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
			moduleLoaderAsm.write_offset = hex_value;
			moduleLoaderAsm.asm_counter  = 0;
			
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
			moduleLoaderAsm.write_offset += 8;
			
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
			moduleLoaderAsm.asm_counter  = 0;
			
		}
		
		// Check next few bytes for an opcode
		byte = 0x00;
		uint16_t i=0;
		
		while (i<0xffff) {i++;
			
			file_read_byte(moduleLoaderAsm.page_offset, byte);
			moduleLoaderAsm.page_offset++;
			
			if (asm_decode_instruction(byte) == 1) 
				break;
			
			// Display only 3 lines at a time
			if (moduleLoaderAsm.asm_counter > 2) {
				moduleLoaderAsm.asm_counter  = 0;
				return;
			}
			
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
	
	if (return_value == 0) {
		console.print(msg_file_not_found, sizeof(msg_file_not_found));
		console.printLn();
	}
	
	// Reset the file pointers
	moduleLoaderAsm.asm_counter  = 0;
	moduleLoaderAsm.page_offset  = 0;
	moduleLoaderAsm.write_offset = 0;
	return;
}

uint8_t asm_decode_instruction(char byte) {
	
	// End of file
	if (moduleLoaderAsm.page_offset > 0xfd) {
		moduleLoaderAsm.page_offset == 0xff;
		console.printHex(0xff);
		console.printSpace();
		console.print("end", sizeof("end"));
		
		console.printLn();
		return 1;
	}
	
	// ADD  0x01
	if (byte == 0x01) {
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("add ", sizeof("add "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		console.printSpace();
		
		moduleLoaderAsm.page_offset++;
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printHex(byte);
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// ADD  0x02
	if (byte == 0x02) {
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("add ", sizeof("add "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		console.printSpace();
		
		moduleLoaderAsm.page_offset++;
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		console.printSpace();
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// SUB 0x28
	if (byte == 0x28) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("sub ", sizeof("sub "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		console.printSpace();
		
		moduleLoaderAsm.page_offset++;
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printHex(byte);
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// SUB 0x29
	if (byte == 0x29) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("sub ", sizeof("sub "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		console.printSpace();
		
		moduleLoaderAsm.page_offset++;
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		console.printSpace();
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// MUL 0xF6
	if (byte == 0xF6) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("mul ", sizeof("mul "));
		
		for (uint8_t a=0; a < 3; a++) {
			file_read_byte(moduleLoaderAsm.page_offset, byte);
			console.printChar( byte + 'a' ); console.printChar('x');
			console.printSpace();
			
			moduleLoaderAsm.page_offset++;
		}
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// MOV 0xa0
	if (byte == 0xa0) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("mov ", sizeof("mov "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		console.printSpace();
		
		moduleLoaderAsm.page_offset++;
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printHex(byte);
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// MOV 0xa1
	if (byte == 0xa1) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("mov ", sizeof("mov "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		console.printSpace();
		
		moduleLoaderAsm.page_offset++;
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printChar( byte + 'a' ); console.printChar('x');
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// INT 0xcd
	if (byte == 0xcd) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("int ", sizeof("int "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printHex(byte);
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// JMP 0xea
	if (byte == 0xea) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("jmp ", sizeof("jmp "));
		
		for (uint8_t a=0; a < 4; a++) {
			file_read_byte(moduleLoaderAsm.page_offset, byte);
			console.printHex(byte);
			
			moduleLoaderAsm.page_offset++;
		}
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// JE 0xeb
	if (byte == 0xeb) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("je ", sizeof("je "));
		
		for (uint8_t a=0; a < 4; a++) {
			file_read_byte(moduleLoaderAsm.page_offset, byte);
			console.printHex(byte);
			
			moduleLoaderAsm.page_offset++;
		}
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	
	// JL 0xec
	if (byte == 0xec) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("jl ", sizeof("jl "));
		
		for (uint8_t a=0; a < 4; a++) {
			file_read_byte(moduleLoaderAsm.page_offset, byte);
			console.printHex(byte);
			
			moduleLoaderAsm.page_offset++;
		}
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	
	// JG 0xed
	if (byte == 0xed) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("jg ", sizeof("jg "));
		
		for (uint8_t a=0; a < 4; a++) {
			file_read_byte(moduleLoaderAsm.page_offset, byte);
			console.printHex(byte);
			
			moduleLoaderAsm.page_offset++;
		}
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	
	// CALL 0x9a
	if (byte == 0x9a) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("call ", sizeof("call "));
		
		for (uint8_t a=0; a < 4; a++) {
			file_read_byte(moduleLoaderAsm.page_offset, byte);
			console.printHex(byte);
			
			moduleLoaderAsm.page_offset++;
		}
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	// CALL 0xc3
	if (byte == 0xc3) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("ret ", sizeof("ret "));
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
	
	// PUSH 0x06
	if (byte == 0x06) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("push ", sizeof("push "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printHex(byte);
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	// POP 0x07
	if (byte == 0x07) {
		
		console.printHex( moduleLoaderAsm.page_offset - 1 );
		console.printSpace();
		
		console.print("pop ", sizeof("pop "));
		
		file_read_byte(moduleLoaderAsm.page_offset, byte);
		console.printHex(byte);
		
		moduleLoaderAsm.page_offset++;
		
		console.printLn();
		moduleLoaderAsm.asm_counter++;
		return 0;
	}
	
}


#endif

