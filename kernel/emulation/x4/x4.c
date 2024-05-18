#include <avr/io.h>
#include <kernel/kernel.h>

#include <kernel/emulation/x4/x4.h>


/*

0x90  nop   
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
*/

void EmulateX4(uint8_t* programBuffer, uint32_t programSize) {
    
    //uint8_t executeProgram[] = "";
    //print( executeProgram, sizeof(executeProgram) );
    //printLn();
    
    uint32_t programCounter = 0;
    
    uint8_t reg[32];
    
    // Temporary stack implementation
    uint8_t stack[100];
    uint32_t stack_ptr;
    
    
    while(1) {
        
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
        if (opCode == 0xFA) {
            
            reg[argA]++;
            
            programCounter += 3;
            
            continue;
        }
        
        // Decrement register
        if (opCode == 0xFC) {
            
            reg[argA]--;
            
            programCounter += 3;
            
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
            
            //union Pointer ptr;
            
            //ptr.byte_t[3] = argA;
            //ptr.byte_t[2] = argB;
            //ptr.byte_t[1] = argC;
            //ptr.byte_t[0] = argD;
            
            programCounter = 4;
            
            continue;
        }
        
        
        //
        // Software interrupt
        //
        
        if (opCode == 0xCC) {
            
            if (argA == 0x10) {
                
                printChar( reg[3] );
                
            }
            
            // Return control to the OS
            if (argA == 0x20) {
                return;
            }
            
            programCounter += 2;
            continue;
        }
        
        programCounter++;
        
        continue;
    }
    
    return;
}

