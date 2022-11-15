//
// Simple emulator

// add   0x01 - Add a byte into a register
// add   0x02 - Add a register into a register
// sub   0x28 - Subtract a byte from a register
// sub   0x29 - Subtract a register from a register
// mul   0xF6 - Multiply a register by a register into a register
//
// mov   0xa0 - Move a byte into a register
// mov   0xa1 - Move a register into a register
//
// int   0xcd - Call a software interrupt routine
//
// jmp   0xea - Jump unconditionally to the offset
// je    0xeb - Jump to the offset if AX is equal to BX
// jl    0xec - Jump to the offset if AX is less than BX
// jg    0xed - Jump to the offset if AX is greater than BX
//
// call  0x9a - Call to a function offset
// ret   0xc3 - Return from a function call
//
// push  0x06 - Push register A onto the stack
// pop   0x07 - Pop data off the stack into register A


#define  rAX  0x00
#define  rBX  0x01
#define  rCX  0x02
#define  rDX  0x03
#define  rEX  0x04






uint8_t software_interrupt_handler(uint8_t interrupt_id);

// Call return pointer
WrappedPointer call_return;

// Registers
char reg[31];   // General purpose
char flags[31]; // Flags

// Opcode registers
char opcode;
char opndA;
char opndB;
char opndC;
char opndD;


void emulation_test_scrpt(void) {
	
	// Initiate registers and flags
	for (uint8_t b=0; b < 31; b++) {
		reg[b] = 0x00;
		flags[b] = 0x00;
	}
	
	// Open file and check EXECUTABLE attribute
	char attr = file_get_attribute(console.keyboard_string, 0);
	uint8_t file_state;
	
	file_state = file_open(console.keyboard_string);
	
	if ((file_state != 0) & (attr == 'x')) {
		
		char data_byte=0;
		
		uint32_t file_start = 0;
		uint32_t file_end   = file_start + fs.file_size + (SECTOR_SIZE - 1);
		
		for (uint32_t ip=file_start; ip <= file_end;) {
			
			// Read the opcode
			file_read_byte(ip, opcode);
			
			// ADD - Add a byte to a register
			if (opcode == 0x01) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] += opndB;
				continue;
			}
			// ADD - Add a register to a register
			if (opcode == 0x02) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] += reg[opndB];
				continue;
			}
			// SUB - Subtract a byte from a register
			if (opcode == 0x28) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] -= opndB;
				continue;
			}
			// SUB - Subtract a register from a register
			if (opcode == 0x29) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] -= reg[opndB];
				continue;
			}
			// MUL - Multiply a register by a register into a register
			if (opcode == 0xf6) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				ip++; file_read_byte(ip, opndC);
				reg[opndA] = (reg[opndB] * reg[opndC]);
				continue;
			}
			
			
			
			// MOV - Move byte into a register
			if (opcode == 0xa0) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] = opndB;
				continue;
			}
			// MOV - Move register into a register
			if (opcode == 0xa1) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] = reg[opndB];
				continue;
			}
			
			
			
			// INT - Interrupt routine
			if (opcode == 0xcd) {
				ip++; file_read_byte(ip, opndA);
				if (software_interrupt_handler( opndA ) == 1) {
					console.clearKeyboardString();
					console.printLn();
					return;
				}
				continue;
			}
			
			
			
			// JMP - Jump to non-relative offset
			if (opcode == 0xea) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				ip = (file_start + pointer.address) - 1;
				continue;
			}
			// JE - Jump to the offset if AX is equal to BX
			if (opcode == 0xeb) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				if (reg[opndA] == reg[opndB]) 
					ip = (file_start + pointer.address) - 1;
				continue;
			}
			// JL - Jump to the offset if AX is less than BX
			if (opcode == 0xec) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				if (reg[opndA] < reg[opndB]) 
					ip = (file_start + pointer.address) - 1;
				continue;
			}
			// JG - Jump to the offset if AX is greater than BX
			if (opcode == 0xed) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				if (reg[opndA] > reg[opndB]) 
					ip = (file_start + pointer.address) - 1;
				continue;
			}
			
			
			// Call a function sub routine
			if (opcode == 0x9a) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				
				call_return.address = ip;
				ip = (file_start + pointer.address) - 1;
				
				continue;
			}
			// Return from a function sub routine
			if (opcode == 0xc3) {
				ip = (file_start + call_return.address) - 1;
				continue;
			}
			
			
			
			// Push a register onto the stack
			if (opcode == 0x06) {
				uint32_t ptr = exMem.stack_push(1);
				exMem.write(ptr, reg[ opndA ]);
				continue;
			}
			// Pop a byte off the stack into a register
			if (opcode == 0x07) {
				uint32_t ptr = (exMem._STACK_BEGIN__ + exMem.stack_size());
				exMem.stack_pop(1);
				exMem.read(ptr, reg[ opndA ]);
				continue;
			}
			
			// No opcode shift down
			ip++;
		}
		
		console.clearKeyboardString();
		console.printLn();
		
	} else {
		
		if (console.last_string_length > 1) {
			console.print("Bad cmd or filename", sizeof("Bad cmd or filename"));
			console.printLn();
		}
		
	}
	
	
	
	
	
	return;
}




uint8_t software_interrupt_handler(uint8_t interrupt_id) {
	
	// INT 20 - Return control
	if (opndA == 0x20) return 1;
	
	// INT 10 - Display
#ifdef _CONSOLE_DRIVER__
	if (opndA == 0x10) {
		call_extern((Device)ConsoleLibraryEntryPoint, reg[rAX], (uint8_t&)reg[rDX]);
		return 0;
	}
#endif
	
	// INT 16 - Keyboard
#ifdef _KEYBOARD_HANDLER__
	if (opndA == 0x16) {
		call_extern((Device)keyboardDeviceDriverEntryPoint, reg[rAX], (uint8_t&)reg[rBX]);
		return 0;
	}
#endif
	
	// INT 14 - Network
#ifdef __NETWORK_INTERFACE_CARD__
	if (opndA == 0x14) {
		call_extern((Device)NetworkInterfaceDeviceDriverEntryPoint, reg[rAX], (uint8_t&)reg[rDX]);// Read the network state flag
	}
	
#endif
	
	
	
	return 0;
}








