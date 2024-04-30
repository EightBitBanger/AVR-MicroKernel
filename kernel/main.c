#include <kernel/main.h>


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
    // Display keyboard input hex
    //
    
    /*
    
    uint8_t nameKeyboard[] = "PS2";
	struct Driver* keyboadDevice = (struct Driver*)GetDriverByName( nameKeyboard, sizeof(nameKeyboard) );
	
	uint8_t oldScanCodeLow  = 0;
    uint8_t oldScanCodeHigh = 0;
    
    uint8_t currentScanCodeLow  = 0;
    uint8_t currentScanCodeHigh = 0;
    
    while(1) {
        
        uint8_t hexStringLow[2];
        uint8_t hexStringHigh[2];
        
        
#ifdef BOARD_RETRO_AVR_X4_REV1
        keyboadDevice->read( 0x00001, &currentScanCodeLow );
        keyboadDevice->read( 0x00000, &currentScanCodeHigh );
#endif
        
#ifdef BOARD_RETROBOARD_REV2
        keyboadDevice->read( 0x00000, &currentScanCodeLow );
        keyboadDevice->read( 0x00001, &currentScanCodeHigh );
#endif
        
        if ((oldScanCodeLow  == currentScanCodeLow) & 
            (oldScanCodeHigh == currentScanCodeHigh)) 
            continue;
        
        oldScanCodeLow  = currentScanCodeLow;
        oldScanCodeHigh = currentScanCodeHigh;
        
        int_to_hex_string(oldScanCodeLow,  hexStringLow);
        int_to_hex_string(oldScanCodeHigh, hexStringHigh);
        
        print(hexStringLow, sizeof(hexStringLow)+1);
        printChar(' ');
        print(hexStringHigh, sizeof(hexStringHigh)+1);
        printLn();
        
        continue;
    }
    
    */
    
    
    
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
    // Speaker beep if available
    //
    
#ifdef BOARD_RETRO_AVR_X4_REV1
    
    uint16_t duration  = 200;
    uint16_t frequency = 20000;
    
    spkBeep(duration, frequency);
    
#endif
    
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
     
    
    //
    // Packet router
    //
    
#ifdef NETWORK_APPLICATION_PACKET_ROUTER
    
    InitiateRouter();
    
#endif
    
#ifdef NETWORK_APPLICATION_SERVER
    
    InitiateServer();
    
#endif
    
#ifndef DONT_INCLUDE_CONSOLE_COMMANDS
    
    // Console commands to include in the kernel
  #ifdef INCLUDE_KERNEL_APPLICATIONS
    
    registerCommandList();
    registerCommandDevice();
    registerCommandCLS();
    registerCommandEDIT();
    
  #endif
    
  #ifdef INCLUDE_NETWORK_APPLICATIONS
    
    registerCommandNet();
    
  #endif
    
  #ifdef INCLUDE_FILE_SYSTEM_APPLICATIONS
    
    registerCommandCAP();
    registerCommandCD();
    registerCommandDIR();
    registerCommandMK();
    registerCommandRM();
    registerCommandRN();
    registerCommandRepair();
    registerCommandFormat();
    
  #endif
    
#endif
    
    
    
    _delay_ms(100);
    
    printPrompt();
    
    while(1) {
        
        consoleUpdate();
        
    }
    
    
    return 0;
}
