#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/graph/graph.h>

extern struct Driver* displayDevice;


void functionGRAPH(uint8_t* param, uint8_t param_length) {
    
    uint16_t value = 1;
    
    while (1) {
        
        displayDevice->write(0x00000, 1);
        
        displayDevice->write(0x00001, 0);
        displayDevice->write(0x00002, 0);
        
        for (uint32_t l=0; l < 8; l++) {
            
            displayDevice->write(0x00001, l);
            
            for (uint32_t c=0; c < 128; c++) {
                
                value += (c * 32) / (c + 1);
                
                displayDevice->write(0x0000a + c, value);
                
            }
            
        }
        
    }
    
    return;
}

void registerCommandGRAPHICS(void) {
    
    uint8_t graphCommandName[] = "graph";
    
    ConsoleRegisterCommand(graphCommandName, sizeof(graphCommandName), functionGRAPH);
    
    return;
}
