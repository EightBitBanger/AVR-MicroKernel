#include <avr/io.h>
//#include <stdlib.h>

#include <util/delay.h>

#include <kernel/bus/board.h>

#include <kernel/kernel.h>

// Included console commands
#include <kernel/commands/list.h>
#include <kernel/commands/device.h>
#include <kernel/commands/cls.h>
#include <kernel/commands/net.h>

#include <kernel/commands/fs/cap.h>
#include <kernel/commands/fs/cd.h>
#include <kernel/commands/fs/dir.h>
#include <kernel/commands/fs/mk.h>
#include <kernel/commands/fs/rm.h>
#include <kernel/commands/fs/rn.h>
#include <kernel/commands/fs/repair.h>
#include <kernel/commands/fs/format.h>


// Included drivers
#include <drivers/display/LiquidCrystalDisplayController/main.h>
#include <drivers/keyboard/ps2/main.h>
#include <drivers/network/NIC/main.h>

// RAM TOP ADDRESS 0x78000

int main(void) {
    
    // Zero the system bus
    bus_control_zero();
    bus_address_zero();
    
    // Allow board some time to stabilize
    _delay_ms(1000);
    
    // Initiate device drivers here
	initiateDisplayDriver();      // 20x4 LCD Display
	initiatePS2Driver();          // PS/2 Port
	initiateNetworkDriver();      // UART Network Card
	
	// Initiate the kernel
	InitiateDeviceTable();
    // Network layer
    ntInitiate();
    // Console
    consoleInitiate();
    
    // Console commands to include in the kernel
    registerCommandList();
    registerCommandDevice();
    registerCommandCLS();
    registerCommandNet();
    
    registerCommandCAP();
    registerCommandCD();
    registerCommandDIR();
    registerCommandMK();
    registerCommandRM();
    registerCommandRN();
    registerCommandRepair();
    registerCommandFormat();
    
    
    
    //
    // Allocate external memory
    //
    
    struct Bus memoryBus;
	
	memoryBus.read_waitstate  = 20;
	memoryBus.write_waitstate = 20;
	
	// Check kernel was initiated
	uint8_t kernerBuffer[] = {0x00};
	
	bus_read_memory(&memoryBus, EXTERNAL_MEMORY_BEGIN, &kernerBuffer[0]);
	
	// Check cold start, was kernel already initiated
	if (kernerBuffer[0] != 'K') {
        
        ConsoleSetBlinkRate(0);
        
        _delay_ms(1500);
        
        uint32_t total = 0;
        uint16_t counter = 0;
        
        uint8_t buffer=0;
        
        for (uint32_t address=0x00000000; address < 0xffffffff; address++) {
            
            // Test byte 0xff
            bus_write_memory(&memoryBus, address, 0xff);
            
            bus_read_memory(&memoryBus, address, &buffer);
            
            if (buffer != 0xff) 
                break;
            
            total++;
            
            counter++;
            if (counter < 512) 
                continue;
            
            counter = 0;
            
            ConsoleSetCursor(0, 0);
            
            uint8_t totalString[10];
            uint8_t place = int_to_string(total, totalString);
            print(totalString, place + 1);
            
            continue;
        }
        
        uint8_t totalString[10];
        uint8_t place = int_to_string(total, totalString);
        
        ConsoleSetCursor(0, 0);
        print(totalString, place + 1);
        printSpace(1);
        
        uint8_t byteFreeString[] = "bytes free";
        print(byteFreeString, sizeof(byteFreeString));
        
        printLn();
        
	}
    
    
    ConsoleSetBlinkRate(35);
    
    
    //
    // Initiate kernel space in external memory
    //
    
    bus_write_memory( &memoryBus, EXTERNAL_MEMORY_BEGIN, 'K');
    
    
    
    
    //
    // Find an active storage device to call the file system root directory
    //
    
    for (uint8_t d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        fsSetCurrentDevice( d );
        
        uint8_t deviceHeader = fsGetDeviceHeaderByte(0);
        
        if (deviceHeader != 0x13) 
            continue;
        
        // Set the root directory
        fsSetCurrentDevice( d );
        uint8_t prompt[] = {('A' + d), '>'};
        ConsoleSetPrompt( prompt, sizeof(prompt) + 1 );
        
        break;
    }
    
    _delay_ms(300);
    
    printPrompt();
    
    while(1) {
        
        consoleUpdate();
        
    }
    
    
    return 0;
}
