#include <kernel/main.h>

void taskfunc(void) {
    
    printc("Asshole", 1);
    printLn();
    
    return;
}



int main(void) {
    
    // Zero the system bus
    bus_control_zero();
    bus_address_zero();
    
    // Allow board some time to stabilize
    _delay_ms(1800);
    
    // Device drivers
	initiateDisplayDriver();      // 20x4 LCD Display
	initiatePS2Driver();          // PS/2 Port
	initiateNetworkDriver();      // UART Network Card
	initiateSpeakerDriver();      // On-Board speaker
	
	// Initiate kernel sub systems
	InitiateDeviceTable();
    
    ntInitiate();         // Network support
    
    consoleInitiate();    // Command console shell
    
    schedulerInitiate();  // Scheduler/timer
    
    
    //
    // Kernel version
    
    uint8_t kernelHelloWorldString[] = "kernel v0.0.1";
    print(kernelHelloWorldString, sizeof(kernelHelloWorldString));
    
    ConsoleSetCursor(1, 0);
    
    
    //
    // Allocate external memory
    //
    
    struct Bus memoryBus;
	
	memoryBus.read_waitstate  = 4;
	memoryBus.write_waitstate = 4;
	
	
    ConsoleSetBlinkRate(0);
    
    uint16_t counter=0;
    uint32_t address=0;
    
    uint8_t buffer=0;
    
    for (address=0x00000000; address < 0xffffffff; address++) {
        
        bus_write_memory(&memoryBus, address, 0x55);
        
        bus_read_memory(&memoryBus, address, &buffer);
        
        if (buffer != 0x55) break;
        
        counter++;
        if (counter < 256) 
            continue;
        
        counter = 0;
        
        ConsoleSetCursor(1, 0);
        
        uint8_t totalString[10];
        uint8_t place = int_to_string(address, totalString);
        print(totalString, place + 1);
        
        continue;
    }
    
    uint8_t totalString[10];
    uint8_t place = int_to_string(address, totalString);
    
    ConsoleSetCursor(1, 0);
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
    
    registerCommandList();
    registerCommandDevice();
    registerCommandCLS();
    registerCommandEDIT();
    registerCommandAssembly();
    registerCommandTASK();
    
  #endif
    
  #ifdef INCLUDE_NETWORK_APPLICATIONS
    
    registerCommandNet();
    
  #endif
    
  #ifdef INCLUDE_FILE_SYSTEM_APPLICATIONS
    
    registerCommandDIR();
    registerCommandCD();
    registerCommandCAP();
    registerCommandMK();
    registerCommandRM();
    registerCommandRN();
    registerCommandATTRIB();
    registerCommandRepair();
    registerCommandFormat();
    
  #endif
    
#endif
    
    // Launch the command console task
    uint8_t taskname[] = "command";
    task_create(taskname, sizeof(taskname), consoleRunShell, TASK_PRIORITY_REALTIME, TASK_TYPE_SERVICE);
    
    
    uint8_t testtaskname[] = "test";
    
    task_create(testtaskname, sizeof(testtaskname), taskfunc, TASK_PRIORITY_HALT, TASK_TYPE_USER);
    
    
    
    printPrompt();
    
    schedulerStart();
    
    while(1) {
        
        //consoleRunShell();
        
        continue;
    }
    
    schedulerStop();
    
    return 0;
}
