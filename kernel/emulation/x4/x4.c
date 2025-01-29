#include <stdint.h>

#include <kernel/delay.h>
#include <kernel/kernel.h>

#include <kernel/emulation/x4/x4.h>

/*

0x90  nop
0x86  lda
0x87  sta
0x88  mov
0x89  mov
0xFA  inc
0xFC  dec

0x00  add
0x80  sub
0xF6  mul
0xF4  div

0x0F  pop
0xF0  pus

0x38  cmp

0xFE  jmp

0xCC  int
0xCD  int

*/

uint8_t programBuffer[256];
uint32_t programSize;

uint8_t param_string[16];
uint32_t param_length;


// Emulation


void EmulatorSetProgram(uint8_t* buffer, uint32_t size) {
    
    for (uint32_t i=0; i < size; i++) 
        programBuffer[i] = buffer[i];
    
    programSize = size;
    
    return;
}

void EmulatorSetParam(uint8_t* buffer, uint32_t length) {
    
    for (uint32_t i=0; i < length; i++) 
        param_string[i] = buffer[i];
    
    param_length = length;
    
    return;
}

void EmulatorStart(uint8_t* filename, uint8_t nameLength, uint16_t priority, uint8_t privilege, uint8_t type) {
    
    TaskCreate(filename, nameLength, EmulateX4, priority, privilege, type);
    
    return;
}





void EmulateX4(uint8_t messages) {
    
    struct Bus mem_bus;
    mem_bus.read_waitstate = 2;
    mem_bus.write_waitstate = 2;
    
    uint32_t programCounter        = 0;
    uint32_t programCounterReturn  = 0;
    
    uint8_t reg[32];
    
    // Temporary stack implementation
    uint8_t stack[100];
    uint32_t stack_ptr;
    
    // Flags
    uint8_t flag_compare = 1;
    
    uint8_t consoleWritten = 1;
    
    
    while(1) {
        
        uint8_t opCode = programBuffer[programCounter];
        uint8_t argA   = programBuffer[programCounter + 1];
        uint8_t argB   = programBuffer[programCounter + 2];
        uint8_t argC   = programBuffer[programCounter + 3];
        uint8_t argD   = programBuffer[programCounter + 4];
        
        // NOP
        if (opCode == NOP) {
            programCounter += 1;
            continue;
        }
        
        // IN Read a byte from IO space
        if (opCode == IN) {
            
            
            programCounter += 5;
            continue;
        }
        
        // OUT Write a byte into IO space
        if (opCode == OUT) {
            
            
            programCounter += 5;
            continue;
        }
        
        // LDA Load from memory to DX register
        if (opCode == LDA) {
            union Pointer ptr;
            ptr.byte_t[0] = argA;
            ptr.byte_t[1] = argB;
            ptr.byte_t[2] = argC;
            ptr.byte_t[3] = argD;
            
            bus_read_memory(&mem_bus, ptr.address, &reg[3]);
            
            programCounter += 5;
            continue;
        }
        
        // STA Store DX register to memory
        if (opCode == STA) {
            union Pointer ptr;
            ptr.byte_t[0] = argA;
            ptr.byte_t[1] = argB;
            ptr.byte_t[2] = argC;
            ptr.byte_t[3] = argD;
            
            bus_write_memory(&mem_bus, ptr.address, reg[3]);
            
            programCounter += 5;
            continue;
        }
        
        // MOV register into register
        if (opCode == MOVB) {
            reg[argA] = reg[argB];
            programCounter += 3;
            continue;
        }
        
        // MOV byte into register
        if (opCode == MOV) {
            reg[argA] = argB;
            programCounter += 3;
            continue;
        }
        
        // Increment register
        if (opCode == INC) {
            reg[argA]++;
            programCounter += 2;
            continue;
        }
        
        // Decrement register
        if (opCode == DEC) {
            reg[argA]--;
            programCounter += 2;
            continue;
        }
        
        
        //
        // Math
        //
        
        // Add BX into AX
        if (opCode == ADD) {
            reg[0] += reg[1];
            programCounter += 1;
            continue;
        }
        
        // Sub BX from AX
        if (opCode == SUB) {
            reg[0] -= reg[1];
            programCounter += 1;
            continue;
        }
        
        // Multiply BX and CX into AX
        if (opCode == MUL) {
            reg[0] = reg[1] * reg[2];
            programCounter += 1;
            continue;
        }
        
        // Divide BX by CX into AX
        // Remainder into dx
        if (opCode == DIV) {
            //reg[argA] ;
            programCounter += 1;
            continue;
        }
        
        
        //
        // Stack memory
        //
        
        // POP from stack into a register
        if (opCode == POP) {
            stack_ptr--;
            reg[argA] = stack[stack_ptr];
            programCounter += 2;
            continue;
        }
        
        // PUSH to stack from a register
        if (opCode == PUSH) {
            stack[stack_ptr] = reg[argA];
            stack_ptr++;
            programCounter += 2;
            continue;
        }
        
        
        // Compare byte
        if (opCode == CMP) {
            if (reg[argA] == argB) {
                flag_compare = 1;
            } else {
                flag_compare = 0;
            }
            programCounter += 3;
            continue;
        }
        
        // Compare registers
        if (opCode == CMPR) {
            if (reg[argA] == reg[argB]) {
                flag_compare = 1;
            } else {
                flag_compare = 0;
            }
            programCounter += 3;
            continue;
        }
        
        
        // JUMP
        if (opCode == JMP) {
            union Pointer ptr;
            ptr.byte_t[3] = argA;
            ptr.byte_t[2] = argB;
            ptr.byte_t[1] = argC;
            ptr.byte_t[0] = argD;
            programCounter = ptr.address;
            continue;
        }
        
        // Conditional jump
        
        // Jump if EQUAL
        if (opCode == JE) {
            if (flag_compare == 1) {
                union Pointer ptr;
                ptr.byte_t[3] = argA;
                ptr.byte_t[2] = argB;
                ptr.byte_t[1] = argC;
                ptr.byte_t[0] = argD;
                programCounter = ptr.address;
                continue;
            }
            
            programCounter += 5;
            continue;
        }
        
        // Jump if NOT EQUAL
        if (opCode == JNE) {
            if (flag_compare == 0) {
                union Pointer ptr;
                ptr.byte_t[3] = argA;
                ptr.byte_t[2] = argB;
                ptr.byte_t[1] = argC;
                ptr.byte_t[0] = argD;
                programCounter = ptr.address;
                continue;
            }
            
            programCounter += 5;
            continue;
        }
        
        // CALL
        if (opCode == CALL) {
            union Pointer ptr;
            ptr.byte_t[3] = argA;
            ptr.byte_t[2] = argB;
            ptr.byte_t[1] = argC;
            ptr.byte_t[0] = argD;
            programCounterReturn = programCounter + 5;
            programCounter = ptr.address;
            continue;
        }
        
        // RET return from call
        if (opCode == RET) {
            
            programCounter = programCounterReturn;
            continue;
        }
        
        
        //
        // Software interrupt
        //
        
        // Clear interrupt flag
        if (opCode == 0xFA) {
            
            __asm__("cli");
            
            programCounter += 1;
            
            continue;
        }
        
        // Set interrupt flag
        if (opCode == 0xFB) {
            
            __asm__("sei");
            
            programCounter += 1;
            
            continue;
        }
        
        // Interrupt routines
        if ((opCode == 0xCC) | (opCode == 0xCD)) {
            
            //
            // Display
            //
            
            if (argA == 0x10) {
                
                // ah=01 - Print string
                if (reg[rAH] == 0x09) {
                    
                    union Pointer dataOffset;
                    dataOffset.byte_t[3] = 0;
                    dataOffset.byte_t[2] = 0;
                    dataOffset.byte_t[1] = reg[rDL];
                    dataOffset.byte_t[0] = reg[rDH];
                    for (uint32_t i=0; i < programSize; i++) {
                        
                        if (programBuffer[dataOffset.address + i] == '\0') 
                            break;
                        
                        if (programBuffer[dataOffset.address + i] == '\n') {
                            printLn();
                            continue;
                        }
                        
                        printChar(programBuffer[dataOffset.address + i]);
                        
                    }
                    
                    consoleWritten = 1;
                }
                
                // ah=03 - Clear display
                if (reg[rAH] == 0x03) {
                    ConsoleClearScreen();
                    ConsoleSetCursor(0, 0);
                    
                    consoleWritten = 0;
                }
                
            }
            
            
            //
            // Network
            //
            
            /*
            
            if (argA == 0x14) {
                
                // ah=02 Send network message
                if (reg[rAH] == 0x02) {
                    
                    union Pointer dataOffset;
                    dataOffset.byte_t[3] = 0;
                    dataOffset.byte_t[2] = 0;
                    dataOffset.byte_t[1] = reg[2];
                    dataOffset.byte_t[0] = reg[3];
                    
                    ntSend(&programBuffer[ dataOffset.address ], reg[1]);
                    
                    consoleWritten = 0;
                    
                }
                
            }
            
            */
            
            
            
            //
            // Operating system routines
            //
            
            if (argA == 0x47) {
                
                // Dynamically load a driver
                // onto the driver table
                if (reg[rAH] == 0x0A) {
                    
                    int8_t status = LoadLibrary(param_string, param_length);
                    
                    if (status > 0) {
                        
                        reg[rBL] = 1;
                        
                    } else {
                        
                        if (status == -2) {
                            
                            reg[rBL] = 2;
                            
                        } else {
                            
                            reg[rBL] = 0;
                        }
                        
                    }
                    
                }
                
                
                
            }
            
            
            
            //
            // Storage / File system access
            //
            
            if (argA == 0x13) {
                
                // ah = 3Ch Create a file
                if (reg[rAH] == 0x3C) {
                    
                    union Pointer dataOffset;
                    dataOffset.byte_t[3] = 0;
                    dataOffset.byte_t[2] = 0;
                    dataOffset.byte_t[1] = reg[rCL];
                    dataOffset.byte_t[0] = reg[rCH];
                    
                    uint32_t fileAddress = fsFileCreate(param_string, param_length, dataOffset.address);
                    
                    // Set return value
                    if (fileAddress != 0) {
                        
                        reg[rBL] = 1;
                        
                        consoleWritten = 0;
                        
                        fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), fileAddress);
                        
                    } else {
                        
                        reg[rBL] = 0;
                    }
                    
                }
                
                // ah = 39h Create a directory
                if (reg[rAH] == 0x39) {
                    
                    uint32_t directoryAddress = fsDirectoryCreate(param_string, param_length);
                    
                    // Set return value
                    if (directoryAddress != 0) {
                        
                        reg[rBL] = 1;
                        
                        consoleWritten = 0;
                        
                        fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), directoryAddress);
                        
                    } else {
                        
                        reg[rBL] = 0;
                    }
                    
                }
                
                // ah = 41h Delete a file
                if (reg[rAH] == 0x41) {
                    
                    // Delete file
                    uint32_t fileAddress = fsFileDelete(param_string, param_length);
                    
                    if (fileAddress != 0) {
                        
                        reg[rBL] = 1;
                        
                        consoleWritten = 0;
                        
                        fsDirectoryRemoveFile(fsWorkingDirectoryGetAddress(), fileAddress);
                        
                    } else {
                        
                        reg[rBL] = 0;
                    }
                    
                }
                
                // ah = 3Ah Delete a directory
                if (reg[rAH] == 0x3A) {
                    
                    uint32_t directoryAddress = fsDirectoryDelete(param_string, param_length);
                    
                    if (directoryAddress != 0) {
                        
                        reg[rBL] = 1;
                        
                        consoleWritten = 0;
                        
                        fsDirectoryRemoveFile(fsWorkingDirectoryGetAddress(), directoryAddress);
                        
                    } else {
                        
                        reg[rBL] = 0;
                    }
                    
                }
                
                
                // ah = 3Bh Change the current working directory
                if (reg[rAH] == 0x3B) {
                    
                    consoleWritten = 0;
                    
                    reg[rBL] = 1;
                }
                
                
                // ah = 3Ch Show current directory path
                if (reg[rAH] == 0x3F) {
                    
                    consoleWritten = 1;
                    
                    reg[rBL] = 1;
                }
                
            }
            
            
            // Return control to the OS
            if (argA == 0x20) {
                
                if (consoleWritten == 1) 
                    printLn();
                
                for (uint32_t i=0; i < 16; i++) 
                    param_string[i] = ' ';
                
                return;
            }
            
            programCounter += 2;
            continue;
        }
        
        programCounter++;
        
        // End of the line!
        if (programCounter >= programSize) 
            break;
        
        continue;
    }
    
    if (consoleWritten == 1) 
        printLn();
    
    for (uint32_t i=0; i < 16; i++) 
        param_string[i] = ' ';
    
    return;
}

