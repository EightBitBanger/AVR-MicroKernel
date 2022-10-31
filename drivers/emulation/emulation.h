//
// Simple x86 emulator

void emulation_test_scrpt(void) {
	
	// Function not found. Check if the filename exists
	if ((fs.file_open(console.keyboard_string) != 0) & (console.last_string_length > 1) & (fs.file_get_attribute(console.keyboard_string, 0) == 'x')) {
		
		// File found with executable attribute
		
		// Simple instruction set
		// 
		// add   0x01 - Add a register into a register
		// add   0x02 - Add a byte into a register
		// 
		// mov   0xa0 - Move a byte into a register
		// movr  0xa1 - Move a register into a register
		// 
		// int   0x3d - Run an interrupt routine
		// 
		// jmp   0xea - Jump unconditionally to the offset
		// je           Jump to the offset if register A is equal to B
		// jg           Jump to the offset if register A is greater than B
		// jl           Jump to the offset if register A is less than B
		// 
		// call  0x9a - Call to a function offset
		// ret          Return from a function call
		// 
		// push  0x06 - Push register A onto the stack
		// pop   0x07 - Pop data off the stack into register A
		
		// General purpose registers
		char reg[31];
		
		for (uint8_t b=0; b < 31; b++) 
			reg[b] = 0x00;
		
		char opcode=0x00;
		char operandA=0x00;
		char operandB=0x00;
		char operandC=0x00;
		char operandD=0x00;
		
		uint32_t file_start = (SECTOR_SIZE + 1);
		uint32_t file_end   = file_start + fs.file_size;
		
		for (uint32_t ip=file_start; ip <= file_end; ip++) {
			
			// Get the op-code
			fs.file_read_byte(ip, opcode);
			
			//
			// ADD - Add a register to a register
			if (opcode == 0x01) {
				ip++; fs.file_read_byte(ip, operandA);
				ip++; fs.file_read_byte(ip, operandB);
				
				reg[operandA] += reg[operandB];
				
				continue;
			}
			
			//
			// ADD - Add a byte to a register
			if (opcode == 0x02) {
				ip++; fs.file_read_byte(ip, operandA);
				ip++; fs.file_read_byte(ip, operandB);
				
				reg[operandA] += operandB;
				
				continue;
			}
			
			//
			// MOV - Move byte into a register
			if (opcode == 0xa0) {
				ip++; fs.file_read_byte(ip, operandA);
				ip++; fs.file_read_byte(ip, operandB);
				
				reg[operandA] = operandB;
				
				continue;
			}
			
			//
			// MOV - Move register into a register
			if (opcode == 0xa1) {
				ip++; fs.file_read_byte(ip, operandA);
				ip++; fs.file_read_byte(ip, operandB);
				
				reg[operandA] = reg[operandB];
				
				continue;
			}
			
			//
			// INT - Interrupt routine
			if ((opcode == 0x3d) | (opcode == 0xcd)) {
				ip++; fs.file_read_byte(ip, operandA);
				
				// INT 10 - Display call
				if (operandA == 0x10) {console.printChar( reg[0] ); continue;}
				
				// INT 20 - Program termination call
				if (operandA == 0x20) break;
				
				continue;
			}
			
			//
			// JMP - Jump to non-relative offset
			if (opcode == 0xea) {
				
				WrappedPointer pointer;
				
				ip++; fs.file_read_byte(ip, pointer.byte[3]);
				ip++; fs.file_read_byte(ip, pointer.byte[2]);
				ip++; fs.file_read_byte(ip, pointer.byte[1]);
				ip++; fs.file_read_byte(ip, pointer.byte[0]);
				
				ip = (file_start + pointer.address) - 1;
				
				continue;
			}
			
			//
			// Push a register onto the stack
			if (opcode == 0x06) {
				uint32_t ptr = exMem.stack_push(1);
				exMem.write(ptr, reg[ operandA ]);
				continue;
			}
			
			//
			// Pop a byte off the stack into a register
			if (opcode == 0x07) {
				uint32_t ptr = (exMem._STACK_BEGIN__ + exMem.stack_size());
				exMem.stack_pop(1);
				exMem.read(ptr, reg[ operandA ]);
				continue;
			}
			
		}
		
		console.clearKeyboardString();
		console.printLn();
		
	} else {
		if (console.last_string_length > 1) {
			console.print("Bad cmd or filename", sizeof("Bad cmd or filename"));
			console.printLn();
		}
	}
	
	
	
	
	
	
}

