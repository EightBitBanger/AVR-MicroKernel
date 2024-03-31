#include <avr/io.h>
//#include <stdlib.h>

#include <util/delay.h>


#define BOARD_RETROBOARD_REV2

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

int main(void) {
    
    // Zero the system bus
    bus_address_zero();
    bus_control_zero();
    
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
	
	memoryBus.read_waitstate  = 1;
	memoryBus.write_waitstate = 1;
	
	// Check kernel was initiated
	uint8_t kernerBuffer[] = {0x00};
	
	bus_read_byte(&memoryBus, EXTERNAL_MEMORY_BEGIN, &kernerBuffer[0]);
	
	// Check cold start, was kernel already initiated
	if (kernerBuffer[0] != 'K') {
        
        ConsoleSetBlinkRate(0);
        
        _delay_ms(1000);
        
        uint32_t total = 0;
        uint8_t counter = 0;
        
        for (uint32_t address=EXTERNAL_MEMORY_BEGIN; address < PERIPHERAL_ADDRESS_BEGIN; address++) {
            
            // Test byte 0xff
            bus_write_byte(&memoryBus, address, 0x55);
            
            uint8_t buffer=0;
            bus_read_byte(&memoryBus, address, &buffer);
            
            if (buffer != 0x55) 
                break;
            
            // Test byte 0x00
            bus_write_byte(&memoryBus, address, 0xaa);
            
            bus_read_byte(&memoryBus, address, &buffer);
            
            if (buffer != 0xaa) 
                break;
            
            total++;
            
            counter++;
            if (counter < 100) 
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
    
    bus_write_byte( &memoryBus, EXTERNAL_MEMORY_BEGIN, 'K');
    
    
    
    
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
