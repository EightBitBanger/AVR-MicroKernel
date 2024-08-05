#include <kernel/main.h>


void spk_beep(uint16_t duration, uint16_t frequency) {
    
    struct Bus spkBus;
    spkBus.read_waitstate  = 1000;
    spkBus.write_waitstate = 1000;
    
    
    for (uint16_t i=0; i < duration; i++) {
        
        bus_write_io(&spkBus, 0x60000, 0xff);
        
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
    _delay_ms(1000);
    
    // Device drivers
	initiateDisplayDriver();      // 20x4 LCD Display
	initiatePS2Driver();          // PS/2 Port
	
	// Initiate core kernel systems
	InitiateDeviceTable();        // Hardware device table
    kernelVectorTableInit();      // Hardware interrupt vector table
    schedulerInit();              // Scheduler sub system
    
#ifdef NETWORK_APPLICATION_PACKET_ROUTER
    
    ntInit();
    
    InitiateRouter();
    
#endif
    
    // Command line interpreter
    cliInit();
    
    
    //
    // Allocate external memory
    //
    
#ifdef _KERNEL_ALLOCATE_EXTERNAL_MEMORY__
    
    AllocateExternalMemory();
    
#endif
    
    //
    // Speaker beep
    //
    
#ifdef BOARD_RETRO_AVR_X4_REV1
    uint16_t frequency      = 14000;
    uint16_t duration       = 300;
    uint16_t duration_long  = 2000;
    
    // Check RAM error
    if (kAllocGetTotal() == 0) {
        
        spk_beep(duration_long, frequency);
        
        _delay_ms(500);
        spk_beep(duration, frequency);
        _delay_ms(500);
        
    }
    
    // Normal beep, all clear to continue
    spk_beep(duration, frequency);
    
#endif
    
    //
    // Initiate kernel sub systems
    
    fsInit();         // File system
    kInit();          // Setup the kernel environment
    driverInit();     // Driver hot loader
    
    
#ifdef NETWORK_APPLICATION_SERVER
    
    ntInit();
    
    InitiateServer();
    
#endif
    
#ifndef DONT_INCLUDE_CONSOLE_COMMANDS
    
  #ifdef INCLUDE_KERNEL_APPLICATIONS
    
    //registerCommandDevice();
    //registerCommandCAP();
    //registerCommandList();
    //registerCommandTASK();
    //registerCommandType();
    
    //registerCommandLD();
    
    //registerCommandEDIT();
    registerCommandAssembly();
    
    //registerCommandCLS();
    
  #endif
    
  #ifdef INCLUDE_NETWORK_APPLICATIONS
    
    //registerCommandNet();
    //registerCommandFTP();
    
  #endif
    
  #ifdef INCLUDE_FILE_SYSTEM_APPLICATIONS
    
    registerCommandLS();
    //registerCommandCOPY();
    registerCommandCD();
    
    //registerCommandMK();
    //registerCommandRM();
    //registerCommandRN();
    //registerCommandMKDIR();
    //registerCommandRMDIR();
    
    //registerCommandAttrib();
    //registerCommandRepair();
    //registerCommandFormat();
    
  #endif
    
#endif
    
    //
    // Boot the kernel
    // 
    
#ifdef _KERNEL_PRINT_VERSION_INFORMATION__
    ConsoleSetBlinkRate( CURSOR_BLINK_RATE );
    
    // Version
    uint8_t kernelHelloWorldString[] = "kernel";
    print(kernelHelloWorldString, sizeof(kernelHelloWorldString));
    printSpace(1);
    
    uint8_t versionMajor = _KERNEL_VERSION_MAJOR__;
    uint8_t versionMinor = _KERNEL_VERSION_MINOR__;
    uint8_t versionPatch = _KERNEL_VERSION_PATCH__;
    
    printChar('v');
    printInt(versionMajor);
    printChar('.');
    printInt(versionMinor);
    printChar('.');
    printInt(versionPatch);
    printLn();
    
#endif
    
    // Drop the initial command prompt
    printPrompt();
    
    
    //
    // Launch system tasks
    
    // Command console
    //uint8_t taskname[] = "command";
    //TaskCreate(taskname, sizeof(taskname), cliRunShell, TASK_PRIORITY_REALTIME, TASK_TYPE_SERVICE);
    
    
    //
    // Hardware interrupt handling
    
    // Enable hardware interrupts
    //  Trigger on the HIGH to LOW transition of PIN2
    InterruptStartHardware();
    
    SetHardwareInterruptService( _ISR_hardware_service_routine );
    
    // Prepare the scheduler and its 
    // associated hardware interrupts
    SchedulerStart();
    
    InterruptStartScheduler();
    InterruptStartTimeCounter();
    
    while(1) {
        
        ClearInterruptFlag();
        
        cliRunShell();
        
        SetInterruptFlag();
        
    }
    
    InterruptStopTimerCounter();
    InterruptStopScheduler();
    InterruptStopHardware();
    
    SchedulerStop();
    
    return 0;
}
