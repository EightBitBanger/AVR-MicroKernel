#include <kernel/main.h>

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
    
    ntInit();                     // Network support
    schedulerInit();              // Scheduler sub system
    consoleInit();                // Command console shell
    kernelVectorTableInit();      // Hardware interrupt vector table
    
    //
    // Allocate external memory
    //
    
    struct Bus memoryBus;
	
	memoryBus.read_waitstate  = 3;
	memoryBus.write_waitstate = 1;
	
    ConsoleSetBlinkRate(0);
    
    uint16_t counter=0;
    uint32_t address=0;
    
    uint8_t buffer=0;
    
    for (address=0x00000000; address < 0xffffffff; address++) {
        
        bus_write_memory(&memoryBus, address, 0x55);
        
        bus_read_memory(&memoryBus, address, &buffer);
        
        if (buffer != 0x55) break;
        
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
    
    ConsoleSetBlinkRate( CURSOR_BLINK_RATE );
    
    //
    // Speaker beep if available
    //
    
#ifdef BOARD_RETRO_AVR_X4_REV1
    
    uint16_t duration  = 120;
    uint16_t frequency = 20000;
    
    spkBeep(duration, frequency);
    
#endif
    
#ifdef NETWORK_APPLICATION_PACKET_ROUTER
    
    InitiateRouter();
    
#endif
    
#ifdef NETWORK_APPLICATION_SERVER
    
    InitiateServer();
    
#endif
    
#ifndef DONT_INCLUDE_CONSOLE_COMMANDS
    
  #ifdef INCLUDE_KERNEL_APPLICATIONS
    
    registerCommandDevice();
    
    registerCommandEDIT();
    registerCommandAssembly();
    
    registerCommandList();
    registerCommandCLS();
    
    registerCommandTASK();
    
  #endif
    
  #ifdef INCLUDE_NETWORK_APPLICATIONS
    
    //registerCommandNet();
    
  #endif
    
  #ifdef INCLUDE_FILE_SYSTEM_APPLICATIONS
    
    registerCommandDIR();
    registerCommandCOPY();
    registerCommandCD();
    
    //registerCommandCAP();
    registerCommandMK();
    registerCommandRM();
    //registerCommandRN();
    registerCommandATTRIB();
    //registerCommandRepair();
    registerCommandFormat();
    
  #endif
    
#endif
    
    //
    // Kernel version
    ConsoleSetCursor(1, 0);
    
    uint8_t kernelHelloWorldString[] = "kernel v0.0.1";
    print(kernelHelloWorldString, sizeof(kernelHelloWorldString));
    printLn();
    
    
    
    //
    // Find an active storage device to call the root directory
    
    for (uint8_t d=1; d <= NUMBER_OF_PERIPHERALS; d++) {
        
        fsSetCurrentDevice( d );
        
        if (fsCheckDeviceReady() == 0) 
            continue;
        
        // Set the root directory
        uint8_t prompt[] = {('A' + d), '>'};
        ConsoleSetPrompt( prompt, sizeof(prompt) + 1 );
        
        // Drop the initial command prompt
        printPrompt();
        
        break;
    }
    
    
    // Launch the command console task
    uint8_t taskname[] = "command";
    TaskCreate(taskname, sizeof(taskname), consoleRunShell, TASK_PRIORITY_REALTIME, TASK_TYPE_SERVICE);
    
    // Enable hardware interrupts
    //  Trigger on the HIGH to LOW transition of PIN2
    InterruptStartHardware();
    
    SetHardwareInterruptServiceA( _ISR_hardware_service_routine );
    
    // Prepare the scheduler and its 
    // associated hardware interrupts
    SchedulerStart();
    
    InterruptStartScheduler();
    InterruptStartTimeCounter();
    
    while(1) {
        
        __asm__("nop");
        
        continue;
    }
    
    InterruptStopTimerCounter();
    InterruptStopScheduler();
    InterruptStopHardware();
    
    SchedulerStop();
    
    return 0;
}
