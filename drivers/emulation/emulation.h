//
// Simple emulator



#define  rAX  0x00
#define  rBX  0x01
#define  rCX  0x02
#define  rDX  0x03






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
	
	// Function not found. Check if the filename exists
	if ((file_open(console.keyboard_string) != 0) & (console.last_string_length > 1) & (file_get_attribute(console.keyboard_string, 0) == 'x')) {
		
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
		
		uint32_t file_start = 0;
		uint32_t file_end   = file_start + fs.file_size * 32;
		
		for (uint32_t ip=file_start; ip <= file_end; ip++) {
			
			// Get the op-code
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
			if (opcode == 0xF6) {
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
				if (software_interrupt_handler( opndA ) == 1) return;
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




uint8_t software_interrupt_handler(uint8_t interrupt_id) {
	
	// INT 20 - Return control
	if (opndA == 0x20) return 1;
	
	
	
	// INT 10 - Display
	if (opndA == 0x10) {
#ifdef _CONSOLE_DRIVER__
		call_extern((Device)ConsoleLibraryEntryPoint, reg[rAX], (uint8_t&)reg[rDX]);
#endif
		return 0;
	}
	
	
	// INT 16 - Keyboard
	if (opndA == 0x16) {
#ifdef _KEYBOARD_HANDLER__
		call_extern((Device)keyboardDeviceDriverEntryPoint, reg[rAX], (uint8_t&)reg[rBX]);
#endif
		return 0;
	}
	
	
	// INT 14 - Network
	if (opndA == 0x14) {
		uint8_t network_packet[] = {reg[rAX], reg[rBX], reg[rCX], reg[rDX]};
		
		// Send packet
		uint8_t flag = 0;
		for (uint8_t i=0; i < sizeof(network_packet);) {	
#ifdef __NETWORK_INTERFACE_CARD__
			call_extern((Device)NetworkInterfaceDeviceDriverEntryPoint, 0x01, flag);
			if (flag == 0) {
				flag = 0xff;
				// Write data to the TX frame buffer
				call_extern((Device)NetworkInterfaceDeviceDriverEntryPoint, 0x04, network_packet[i]);
				call_extern((Device)NetworkInterfaceDeviceDriverEntryPoint, 0x00, flag);
				i++;
			}
#endif
		}
		return 0;
	}
	
	
	
	return 0;
}








