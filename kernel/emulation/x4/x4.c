#include <avr/io.h>
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
0xF0  push  

0xFE  jmp   

0xCC  int   
0xCD  int   

*/

uint8_t EmulateX4(uint8_t* programBuffer, uint32_t programSize) {
    
    struct Bus mem_bus;
    mem_bus.read_waitstate = 2;
    mem_bus.write_waitstate = 1;
    
    uint32_t programCounter        = 0;
    uint32_t programCounterReturn  = 0;
    
    uint8_t reg[32];
    
    // Temporary stack implementation
    uint8_t stack[100];
    uint32_t stack_ptr;
    
    // Flags
    uint8_t interruptFlag = 1;
    
    
    uint8_t consoleWritten = 1;
    
    
    while(1) {
        
        if (interruptFlag == 0) {
            
            
        } else {
            
            
        }
        
        uint8_t opCode = programBuffer[programCounter];
        uint8_t argA   = programBuffer[programCounter + 1];
        uint8_t argB   = programBuffer[programCounter + 2];
        uint8_t argC   = programBuffer[programCounter + 3];
        uint8_t argD   = programBuffer[programCounter + 4];
        
        // NOP
        if (opCode == 0x90) {
            programCounter += 1;
            continue;
        }
        
        // LDA Load from memory to DX register
        if (opCode == 0x86) {
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
        if (opCode == 0x87) {
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
        if (opCode == 0x88) {
            reg[argA] = reg[argB];
            programCounter += 3;
            continue;
        }
        
        // MOV byte into register
        if (opCode == 0x89) {
            reg[argA] = argB;
            programCounter += 3;
            continue;
        }
        
        // Increment register
        if (opCode == 0xFD) {
            reg[argA]++;
            programCounter += 2;
            continue;
        }
        
        // Decrement register
        if (opCode == 0xFC) {
            reg[argA]--;
            programCounter += 2;
            continue;
        }
        
        
        //
        // Math
        //
        
        // Add bx into ax
        if (opCode == 0x00) {
            reg[0] += reg[1];
            programCounter += 1;
            continue;
        }
        
        // Sub bx from ax
        if (opCode == 0x80) {
            reg[0] -= reg[1];
            programCounter += 1;
            continue;
        }
        
        // Multiply bx and cx into ax
        if (opCode == 0xF6) {
            reg[0] = reg[1] * reg[2];
            programCounter += 1;
            continue;
        }
        
        // Divide bx by cx into ax
        // Remainder into dx
        if (opCode == 0xF4) {
            //reg[argA] ;
            programCounter += 1;
            continue;
        }
        
        
        //
        // Stack memory
        //
        
        // POP from stack into a register
        if (opCode == 0x0F) {
            reg[argA] = stack[stack_ptr];
            stack_ptr--;
            programCounter += 2;
            continue;
        }
        
        // PUSH to stack from a register
        if (opCode == 0xF0) {
            stack[stack_ptr] = reg[argA];
            stack_ptr++;
            programCounter += 2;
            continue;
        }
        
        
        // JUMP
        if (opCode == 0xFE) {
            union Pointer ptr;
            ptr.byte_t[3] = argA;
            ptr.byte_t[2] = argB;
            ptr.byte_t[1] = argC;
            ptr.byte_t[0] = argD;
            programCounter = ptr.address;
            continue;
        }
        
        // CALL
        if (opCode == 0x9A) {
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
        if (opCode == 0xCB) {
            programCounter = programCounterReturn;
            continue;
        }
        
        
        //
        // Software interrupt
        //
        
        // Clear interrupt flag
        if (opCode == 0xFA) {
            interruptFlag = 0;
            continue;
        }
        
        // Set interrupt flag
        if (opCode == 0xFB) {
            interruptFlag = 1;
            continue;
        }
        
        if ((opCode == 0xCC) | (opCode == 0xCD)) {
            
            // Display control interrupt routine
            if (argA == 0x10) {
                
                // Print string
                if (reg[0] == 0x09) {
                    
                    union Pointer dataOffset;
                    dataOffset.byte_t[3] = 0;
                    dataOffset.byte_t[2] = 0;
                    dataOffset.byte_t[1] = reg[2];
                    dataOffset.byte_t[0] = reg[3];
                    
                    print( &programBuffer[ dataOffset.address ], reg[1] );
                    
                }
                
                consoleWritten = 1;
            }
            
            // Return control to the OS
            if (argA == 0x20) {
                
                if (consoleWritten == 1) 
                    printLn();
                
                return 1;
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
    
    return 0;
}

