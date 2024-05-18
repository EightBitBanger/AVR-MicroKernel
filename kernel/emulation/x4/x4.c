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
    
    
    
    while(1) {
        
        uint8_t opCode = programBuffer[programCounter];
        uint8_t argA   = programBuffer[programCounter + 1];
        uint8_t argB   = programBuffer[programCounter + 2];
        uint8_t argC   = programBuffer[programCounter + 3];
        uint8_t argD   = programBuffer[programCounter + 4];
        
        // NOP
        if (opCode == 0x90) 
            continue;
        
        // MOV register into register
        if (opCode == 0x88) {
            
            reg[argA] = reg[argB];
            
            continue;
        }
        
        // MOV byte into register
        if (programBuffer[0] == 0x89) {
            
            reg[argA] = argB;
            
            continue;
        }
        
        // Increment register
        if (programBuffer[0] == 0xFA) {
            
            reg[argA]++;
            
            continue;
        }
        
        // Decrement register
        if (programBuffer[0] == 0xFC) {
            
            reg[argA]--;
            
            continue;
        }
        
        
        //
        // Math
        //
        
        // Add bx into ax
        if (programBuffer[0] == 0x00) {
            
            reg[0] += reg[1];
            
            continue;
        }
        
        // Sub bx from ax
        if (programBuffer[0] == 0x80) {
            
            reg[0] -= reg[1];
            
            continue;
        }
        
        // Multiply bx and cx into ax
        if (programBuffer[0] == 0xF6) {
            
            reg[0] = reg[1] * reg[2];
            
            continue;
        }
        
        // Divide bx by cx into ax
        // Remainder into dx
        if (programBuffer[0] == 0xF4) {
            
            //reg[argA] ;
            
            continue;
        }
        
        
        //
        // Stack memory
        //
        
        // POP from stack into a register
        if (programBuffer[0] == 0x0F) {
            
            reg[argA] = stack[stack_ptr];
            stack_ptr--;
            
            continue;
        }
        
        // PUSH to stack from a register
        if (programBuffer[0] == 0xF0) {
            
            stack[stack_ptr] = reg[argA];
            stack_ptr++;
            
            continue;
        }
        
        
        // JUMP
        if (programBuffer[0] == 0xFE) {
            
            stack[stack_ptr] = reg[argA];
            stack_ptr++;
            
            continue;
        }
        
        
        
        0xFE  jmp
        
        0xCC  int
        
        programSize++;
        
        continue;
    }
    
    return;
}

