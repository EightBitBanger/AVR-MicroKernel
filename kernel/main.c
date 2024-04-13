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


int main(void) {
    
    // Zero the system bus
    bus_control_zero();
    bus_address_zero();
    
    // Allow board some time to stabilize
    _delay_ms(1500);
    
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
	
	memoryBus.read_waitstate  = 4;
	memoryBus.write_waitstate = 1;
	
	
    ConsoleSetBlinkRate(0);
    
    uint16_t counter=0;
    uint32_t address=0;
    
    for (address=0x00000; address < 0xfffff; address++) {
        
        bus_write_memory(&memoryBus, address, 0xaa);
        
        uint8_t buffer=0;
        bus_read_memory(&memoryBus, address, &buffer);
        
        if (buffer != 0xaa) 
            break;
        
        
        
        counter++;
        if (counter > 255) {
            counter = 0;
            
            bus_control_zero();
            bus_address_zero();
            
            ConsoleSetCursor(0, 0);
            
            uint8_t totalString[40];
            uint8_t place = int_to_string(address, totalString);
            print(totalString, place + 1);
            
            bus_control_zero();
            bus_address_zero();
            
        }
        
        continue;
    }
    
    uint8_t totalString[40];
    uint8_t place = int_to_string(address, totalString);
    
    ConsoleSetCursor(0, 0);
    print(totalString, place + 1);
    printSpace(1);
    
    //uint8_t byteFreeString[] = "bytes free";
    //print(byteFreeString, sizeof(byteFreeString));
    
    printLn();
    
    ConsoleSetBlinkRate(35);
    
    
    //
    // Find an active storage device to call the file system root directory
    //
    
    for (uint8_t d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        fsSetCurrentDevice( d );
        
        uint8_t deviceHeader[2];
        deviceHeader[0] = fsGetDeviceHeaderByte(1);
        deviceHeader[1] = fsGetDeviceHeaderByte(2);
        
        if ((deviceHeader[0] != 'f') | (deviceHeader[1] != 's')) 
            continue;
        
        // Set the root directory
        uint8_t prompt[] = {('A' + d) - 1, '>'};
        ConsoleSetPrompt( prompt, sizeof(prompt) + 1 );
        
        break;
    }
    
    //_delay_ms(300);
    
    printPrompt();
    
    while(1) {
        
        consoleUpdate();
        
    }
    
    
    return 0;
}
