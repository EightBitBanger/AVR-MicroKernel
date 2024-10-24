#include <kernel/kalloc.h>

uint32_t totalAllocatedMemory = 0;

uint32_t kAllocGetTotal(void) {
    
    return totalAllocatedMemory;
}

void AllocateExternalMemory(void) {
    
#ifdef _KERNEL_ALLOCATE_EXTERNAL_MEMORY__
    
    ConsoleCursorDisable();
    
    uint8_t totalString[10];
    
    for (uint8_t i=0; i < 10; i++) 
        totalString[i] = 0x20;
    
    struct Bus memoryBus;
	memoryBus.read_waitstate  = 10;
	memoryBus.write_waitstate = 10;
	
    uint32_t address=0;
    uint16_t counter=0;
    
    uint8_t buffer=0;
    
    for (address=0x00000000; address < 0xffffffff; address+=512) {
        
        bus_raw_write_memory(&memoryBus, address, 0x55);
        bus_raw_read_memory(&memoryBus, address, &buffer);
        
        if (buffer != 0x55) 
            break;
        
        counter++;
        if (counter >= 2048) {
            counter = 0;
            uint8_t place = int_to_string(address, totalString);
            
            ConsoleSetCursor(0, 0);
            
            print(totalString, 10);
            
            ConsoleSetCursor(0, place);
        }
        
        continue;
    }
    
    uint8_t place = int_to_string(address, totalString);
    
    ConsoleSetCursor(0, 0);
    
    print(totalString, place + 1);
    printSpace(1);
    
    uint8_t byteFreeString[] = "bytes free";
    print(byteFreeString, sizeof(byteFreeString));
    
    _delay_ms(100);
    
    totalAllocatedMemory = address;
    
    ConsoleCursorEnable();
    
    ConsoleSetCursor(1, 0);
    
#endif
    
    return;
}


