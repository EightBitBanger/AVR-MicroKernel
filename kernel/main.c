#include <avr/io.h>
//#include <stdlib.h>

#include <kernel/delay.h>

#include <kernel/bus/board.h>

#include <kernel/kernel.h>

// Included console commands
#ifndef NETWORK_APPLICATION_PACKET_ROUTER
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
#endif


// Included drivers
#include <drivers/display/LiquidCrystalDisplayController/main.h>
#include <drivers/keyboard/ps2/main.h>
#include <drivers/network/NIC/main.h>



void spkBeep(uint16_t duration, uint16_t frequency) {
    
    struct Bus speakerBus;
    speakerBus.write_waitstate = 1000;
    
    for (uint16_t i=0; i < duration; i++) {
        
        bus_write_io(&speakerBus, 0x60000, 0x00);
        
        for (uint16_t i=0; i < frequency / 16; i++) 
            __asm__("nop");
        
        continue;
    }
    
    
    return;
}




int main(void) {
    
    // Zero the system bus
    bus_control_zero();
    bus_address_zero();
    
    //uint16_t duration  = 10;
    //uint16_t frequency = 20000;
    
    //spkBeep(duration, frequency);
    
    
    // Allow board some time to stabilize
    _delay_ms(2000);
    
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
    
    
    //
    // Packet router
    //
    
#ifdef NETWORK_APPLICATION_PACKET_ROUTER
    InitiateRouter();
#endif
    
#ifndef NETWORK_APPLICATION_PACKET_ROUTER
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
#endif
    
    
    
    //
    // Allocate external memory
    //
    
    struct Bus memoryBus;
	
	memoryBus.read_waitstate  = 4;
	memoryBus.write_waitstate = 1;
	
	
    ConsoleSetBlinkRate(0);
    
    uint16_t counter=0;
    uint32_t address=0;
    
    uint8_t buffer=0;
    
    for (address=0x00000000; address < 0xffffffff; address++) {
        
        bus_write_memory(&memoryBus, address, 0x55);
        
        _delay_us(1);
        
        bus_read_memory(&memoryBus, address, &buffer);
        
        _delay_us(1);
        
        if (buffer != 0x55) break;
        
        counter++;
        if (counter < 256) 
            continue;
        
        counter = 0;
        
        ConsoleSetCursor(0, 0);
        
        uint8_t totalString[10];
        uint8_t place = int_to_string(address, totalString);
        print(totalString, place + 1);
        
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
    
    ConsoleSetBlinkRate(35);
    
    
    //
    // Find an active storage device to call the file system root directory
    //
    
    for (uint8_t d=1; d <= NUMBER_OF_PERIPHERALS; d++) {
        
        fsSetCurrentDevice( d );
        
        if (fsCheckDeviceReady() == 0) 
            continue;
        
        // Set the root directory
        uint8_t prompt[] = {('A' + d), '>'};
        ConsoleSetPrompt( prompt, sizeof(prompt) + 1 );
        
        break;
    }
    
    
    _delay_ms(100);
    
    printPrompt();
    
    while(1) {
        
        consoleUpdate();
        
    }
    
    
    return 0;
}
