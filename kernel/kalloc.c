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
	memoryBus.read_waitstate  = 1;
	memoryBus.write_waitstate = 1;
	
    uint32_t address=0;
    uint8_t buffer=0;
    uint8_t place;
    uint16_t counter=0;
    
    for (address=0x00000000; address < 0x00040000; address++) {
        
        bus_raw_write_memory(&memoryBus, address, 0x55);
        bus_raw_read_memory(&memoryBus, address, &buffer);
        if (buffer != 0x55) 
            break;
        
        bus_raw_write_memory(&memoryBus, address, 0xAA);
        bus_raw_read_memory(&memoryBus, address, &buffer);
        if (buffer != 0xAA) 
            break;
        
        bus_raw_write_memory(&memoryBus, address, 0xFA);
        bus_raw_read_memory(&memoryBus, address, &buffer);
        if (buffer != 0xFA) 
            break;
        
        bus_raw_write_memory(&memoryBus, address, 0xAF);
        bus_raw_read_memory(&memoryBus, address, &buffer);
        if (buffer != 0xAF) 
            break;
        
        // Print total allocated memory
        
        counter++;
        if (counter < 1024) 
            continue;
        
        counter = 0;
        
        place = int_to_string(address + 1, totalString);
        
        ConsoleSetCursor(0, 0);
        
        print(totalString, 10);
        
        printLn();
        
        ConsoleSetCursor(0, place);
        
    }
    
    totalAllocatedMemory = address + 1;
    
    if (totalAllocatedMemory < 1024) 
        return;
    
    printSpace(1);
    
    uint8_t byteFreeString[] = "bytes free";
    print(byteFreeString, sizeof(byteFreeString));
    
    printLn();
    
#endif
    
    return;
}


