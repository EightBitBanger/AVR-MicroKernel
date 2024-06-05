#include <avr/io.h>

#include <kernel/kernel.h>
#include <kernel/kalloc.h>



void AllocateExternalMemory(void) {
    
#ifdef _KERNEL_ALLOCATE_EXTERNAL_MEMORY__
    
    struct Bus memoryBus;
	
	memoryBus.read_waitstate  = 2;
	memoryBus.write_waitstate = 1;
	
    ConsoleSetBlinkRate(0);
    
    uint16_t counter=0;
    uint32_t address=0;
    
    uint8_t buffer=0;
    
    uint8_t retry = 0;
    
    for (address=0x00000000; address < 0xffffffff; address++) {
        
        // Test 0x55
        bus_write_memory(&memoryBus, address, 0x55);
        
        bus_read_memory(&memoryBus, address, &buffer);
        
        if (buffer != 0x55) {
            
            if (retry > 20) 
                break;
            
            retry++;
            
            address--;
            
            continue;
            
        } else {
            
            retry = 0;
            
        }
        
        if (counter == 0) {
            
            ConsoleSetCursor(0, 0);
            
            uint8_t totalString[10];
            uint8_t place = int_to_string(address, totalString);
            print(totalString, place + 1);
            
        }
        
        counter++;
        
        if (counter == 512) 
            counter = 0;
        
        continue;
    }
    
    uint8_t totalString[10];
    uint8_t place = int_to_string(address, totalString);
    
    ConsoleSetCursor(0, 0);
    
    print(totalString, place + 1);
    printSpace(1);
    
    uint8_t byteFreeString[] = "bytes free";
    print(byteFreeString, sizeof(byteFreeString));
    
    printLn();
    
#endif
    
    return;
}


