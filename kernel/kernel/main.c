#include <kernel/main.h>


int main(void) {
    
    // Zero the system bus
    bus_control_zero();
    bus_address_zero();
    
    // Allow board some time to stabilize
    _delay_ms(1800);
    
    // Initiate device drivers here
	initiateDisplayDriver();      // 20x4 LCD Display
	initiatePS2Driver();          // PS/2 Port
	initiateNetworkDriver();      // UART Network Card
	initiateSpeakerDriver();      // On-Board speaker
	
	// Initiate the kernel
	InitiateDeviceTable();
    // Network layer
    ntInitiate();
    // Console
    consoleInitiate();
    
    
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
        
        //_delay_us(1);
        
        bus_read_memory(&memoryBus, address, &buffer);
        
        //_delay_us(1);
        
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
    
    ConsoleSetBlinkRate( CURSOR_BLINK_RATE );
    
    //
    // Speaker beep if available
    //
    
#ifdef BOARD_RETRO_AVR_X4_REV1
    
    uint16_t duration  = 200;
    uint16_t frequency = 20000;
    
    spkBeep(duration, frequency);
    
#endif
    
    //
    // Find an active storage device to call the root directory
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
    
#ifdef NETWORK_APPLICATION_PACKET_ROUTER
    
    InitiateRouter();
    
#endif
    
#ifdef NETWORK_APPLICATION_SERVER
    
    InitiateServer();
    
#endif
    
#ifndef DONT_INCLUDE_CONSOLE_COMMANDS
    
  #ifdef INCLUDE_KERNEL_APPLICATIONS
    
    //registerCommandList();
    //registerCommandDevice();
    //registerCommandCLS();
    //registerCommandEDIT();
    
  #endif
    
  #ifdef INCLUDE_NETWORK_APPLICATIONS
    
    //registerCommandNet();
    
  #endif
    
  #ifdef INCLUDE_FILE_SYSTEM_APPLICATIONS
    
    registerCommandDIR();
    registerCommandCD();
    //registerCommandCAP();
    registerCommandMK();
    registerCommandRM();
    //registerCommandRN();
    registerCommandATTRIB();
    //registerCommandRepair();
    //registerCommandFormat();
    
  #endif
    
#endif
    
    _delay_ms(100);
    
    printPrompt();
    
    while(1) {
        
        consoleRunShell();
        
    }
    
    
    return 0;
}
