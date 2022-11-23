//
// Simple emulator

// add   0x01 - Add a byte into a register
// addr  0x02 - Add a register into a register
// sub   0x28 - Subtract a byte from a register
// subr  0x29 - Subtract a register from a register
// mul   0xF6 - Multiply a register by a register into a register
//
// mov   0xa0 - Move a byte into a register
// movr  0xa1 - Move a register into a register
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

#ifndef _SOFTWARE_EMULATOR__
#define _SOFTWARE_EMULATOR__

// Instructions
#define add  0x01
#define addr 0x02
#define sub  0x28
#define subr 0x29
#define mul  0xF6

#define mov  0xa0
#define movr 0xa1

#define INT  0xcd

#define jmp  0xea
#define je   0xeb
#define jl   0xec
#define jg   0xed

#define call 0x9a
#define ret  0xc3

#define push 0x06
#define pop  0x07


// Registers
#define  rAX  0x00
#define  rBX  0x01
#define  rCX  0x02
#define  rDX  0x03
#define  rEX  0x04







uint8_t software_interrupt_handler(uint8_t interrupt_id);

// Call return pointer
uint32_t ip;   // Instruction pointer
uint32_t rp;   // Return pointer

// Registers
char reg[31];   // General purpose registers
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
	
	// Reset the instruction pointer
	ip = 0x00;
	
	// Open file and check executable attribute
	Attribute buffer;
	file_get_attribute(console.keyboard_string, buffer);
	
	uint8_t file_state;
	file_state = file_open(console.keyboard_string);
	
	if ((file_state != 0) & (buffer.Executable == 'x')) {
		
		char data_byte=0;
		
		while (1) {
			
			// Read the opcode
			file_read_byte(ip, opcode);
			
			// ADD - Add a byte to a register
			if (opcode == 0x01) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] += opndB;
				ip++;
				continue;
			}
			// ADD - Add a register to a register
			if (opcode == 0x02) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] += reg[opndB];
				ip++;
				continue;
			}
			// SUB - Subtract a byte from a register
			if (opcode == 0x28) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] -= opndB;
				ip++;
				continue;
			}
			// SUB - Subtract a register from a register
			if (opcode == 0x29) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] -= reg[opndB];
				ip++;
				continue;
			}
			// MUL - Multiply a register by a register into a register
			if (opcode == 0xf6) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				ip++; file_read_byte(ip, opndC);
				reg[opndA] = (reg[opndB] * reg[opndC]);
				ip++;
				continue;
			}
			
			
			
			// MOV - Move byte into a register
			if (opcode == 0xa0) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] = opndB;
				ip++;
				continue;
			}
			// MOV - Move register into a register
			if (opcode == 0xa1) {
				ip++; file_read_byte(ip, opndA);
				ip++; file_read_byte(ip, opndB);
				reg[opndA] = reg[opndB];
				ip++;
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
				ip++;
				continue;
			}
			
			
			
			// JMP - Jump to non-relative offset
			if (opcode == 0xea) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				ip = pointer.address - 1;
				continue;
			}
			// JE - Jump to the offset if AX is equal to BX
			if (opcode == 0xeb) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				if (reg[rAX] == reg[rBX]) 
					ip = pointer.address - 1;
				continue;
			}
			// JL - Jump to the offset if AX is less than BX
			if (opcode == 0xec) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				if (reg[rAX] < reg[rBX]) 
					ip = pointer.address - 1;
				continue;
			}
			// JG - Jump to the offset if AX is greater than BX
			if (opcode == 0xed) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				if (reg[rAX] > reg[rBX]) 
					ip = pointer.address - 1;
				continue;
			}
			
			
			// Call a function sub routine
			if (opcode == 0x9a) {
				WrappedPointer pointer;
				ip++; file_read_byte(ip, pointer.byte[3]);
				ip++; file_read_byte(ip, pointer.byte[2]);
				ip++; file_read_byte(ip, pointer.byte[1]);
				ip++; file_read_byte(ip, pointer.byte[0]);
				
				rp = ip;
				ip = pointer.address - 1;
				
				continue;
			}
			// Return from a function sub routine
			if (opcode == 0xc3) {
				ip = rp - 1;
				continue;
			}
			
			
			
			// Push a register onto the stack
			if (opcode == 0x06) {ip++;
				uint32_t ptr = exMem.stack_push(1);
				exMem.write(ptr, reg[ opndA ]);
				continue;
			}
			// Pop a byte off the stack into a register
			if (opcode == 0x07) {ip++;
				uint32_t ptr = (exMem._STACK_BEGIN__ + exMem.stack_size());
				exMem.stack_pop(1);
				exMem.read(ptr, reg[ opndA ]);
				continue;
			}
			
			// No opcode shift to the next opcode
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
	
	uint8_t byte;
	
	// INT 20 - Return control
	if (opndA == 0x20) return 1;
	
	// INT 10 - Display
#ifdef _CONSOLE_DRIVER__
	if (opndA == 0x10) {
		
		// Print string
		if (reg[rAX] == 0x00) {
			for (uint16_t i=0; i < reg[rCX]; i++) {
				file_read_byte(reg[rDX] + i, (char&)byte);
				call_extern((Device)ConsoleLibraryEntryPoint, 0x00, byte);
			}
			return 0;
		}
		
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







#endif



