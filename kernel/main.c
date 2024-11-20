#include <kernel/main.h>

int main(void) {
    
    // Zero the system bus
    bus_initiate();
    
    // Allow board some time to stabilize
    _delay_ms(1000);
    
    // Baked device drivers
	InitBakedDrivers();
	
	// Initiate core kernel systems
	InitiateDeviceTable();        // Hardware device table
    KernelVectorTableInit();      // Hardware interrupt vector table
    SchedulerInit();              // Scheduler sub system
    DriverTableInit();            // Driver hot loader
    
#ifdef NETWORK_APPLICATION_PACKET_ROUTER
    
    ntInit();
    
    InitiateRouter();
    
#endif
    
    cliInit();
    
#ifdef _KERNEL_ALLOCATE_EXTERNAL_MEMORY__
    
    AllocateExternalMemory();
    
#endif
    
    // Speaker beep error codes
    
#ifdef BOARD_RETRO_AVR_X4_REV1
    
    // Check RAM error
    if (kAllocGetTotal() == 0) {
        
        for (uint8_t a=0; a < 7; a++) 
            sysbeep();
        
        _delay_ms(500);
        sysbeep();
        
        _delay_ms(500);
        
    }
    
    // Normal beep, all clear to continue
    sysbeep();
    
#endif
    
    // Initiate kernel sub systems
    
    fsInit();         // File system
    kInit();          // Setup the kernel environment
    
#ifdef NETWORK_APPLICATION_SERVER
    
    ntInit();
    
    InitiateServer();
    
#endif
    
    registerCommandLS();
    registerCommandCD();
    
    //registerCommandEDIT();
    //registerCommandASM();
    
    //registerCommandType();
    registerCommandList();
    //registerCommandTASK();
    //registerCommandTest();
    
    //registerCommandRN();
    //registerCommandCOPY();
    
    //registerCommandNet();
    //registerCommandFormat();
    
    #define DONT_INCLUDE_CONSOLE_COMMANDS
    
    
#ifndef DONT_INCLUDE_CONSOLE_COMMANDS
    
  #ifdef INCLUDE_KERNEL_APPLICATIONS
    
    registerCommandCAP();
    registerCommandList();
    registerCommandTASK();
    registerCommandType();
    
    registerCommandLD();
    
    //registerCommandEDIT();
    //registerCommandAssembly();
    
    registerCommandCLS();
    
    registerCommandTest();
    
  #endif
    
  #ifdef INCLUDE_NETWORK_APPLICATIONS
    
    registerCommandNet();
    //registerCommandFTP();
    
  #endif
    
  #ifdef INCLUDE_FILE_SYSTEM_APPLICATIONS
    
    registerCommandLS();
    registerCommandCD();
    
    //registerCommandMK();
    //registerCommandRM();
    registerCommandRN();
    //registerCommandMKDIR();
    registerCommandCOPY();
    
    registerCommandAttribute();
    //registerCommandRepair();
    registerCommandFormat();
    
  #endif
    
#endif
    
    // Boot the kernel
    
    // Set keyboard interrupt handler
    SetInterruptVector(4, (void(*)())cliRunShell);
    
    // Drop the initial command prompt
    printPrompt();
    
    // Enable hardware interrupt handling
    //  Trigger on the HIGH to LOW transition of PIN2
    InterruptStartHardware();
    
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

