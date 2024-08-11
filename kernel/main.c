#include <kernel/main.h>


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
	initiateDeviceTable();        // Hardware device table
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
    // Speaker beep error codes
    //
    
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
    registerCommandList();
    //registerCommandTASK();
    //registerCommandType();
    
    //registerCommandLD();
    
    //registerCommandEDIT();
    //registerCommandAssembly();
    
    //registerCommandCLS();
    
  #endif
    
  #ifdef INCLUDE_NETWORK_APPLICATIONS
    
    registerCommandNet();
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
    
#ifndef _BOOT_SAFEMODE__
    
    // Boot from device
    uint8_t driversDirName[] = "drivers";
    
    for (uint8_t i=0; i < NUMBER_OF_PERIPHERALS; i++) {
        
        fsSetDeviceLetter(i + 'a');
        
        if (fsCheckDeviceReady() == 0) 
            continue;
        
        // Locate drivers directory
        uint32_t directoryAddress = fsDirectoryExists(driversDirName, sizeof(driversDirName)-1);
        
        // Directory does not exist
        if (directoryAddress == 0) 
            continue;
        
        fsChangeWorkingDirectory(driversDirName, sizeof(driversDirName));
        
        uint32_t numberOfFiles = fsWorkingDirectoryGetFileCount();
        
        for (uint32_t f=0; f < numberOfFiles; f++) {
            
            uint32_t fileAddress = fsWorkingDirectoryFind(f);
            
            if (fileAddress == 0) 
                continue;
            
            uint8_t driverBuffer[30];
            
            fsFileOpen(fileAddress);
            
            fsFileRead(driverBuffer, 30);
            
            fsFileClose();
            
            uint8_t driverFilename[10];
            uint8_t driverFilenameLength=0;
            
            for (uint32_t n=0; n < 10; n++) {
                
                fs_read_byte(fileAddress + FILE_OFFSET_NAME + n, &driverFilename[n]);
                
                if (driverFilename[n] == ' ') {
                    driverFilenameLength = n;
                    break;
                }
                
            }
            
#ifdef _BOOT_DETAILS__
            
            for (uint32_t n=0; n < 10; n++) {
                
                printChar( driverBuffer[n + 2] );
                
                if (driverBuffer[n + 2] == ' ') 
                    break;
            }
            
#endif
            
            // Load the driver
            uint8_t libState = LoadLibrary(driverFilename, driverFilenameLength);
            
#ifdef _BOOT_DETAILS__
            
            if (libState <= 0) {
                uint8_t msgFailedToLoad[] = "... failed";
                print(msgFailedToLoad, sizeof(msgFailedToLoad));
            }
            
            printLn();
            
#endif
            
#ifndef _BOOT_DETAILS__
            
            if (libState <= 0) {
                
                for (uint32_t n=0; n < 10; n++) {
                    
                    printChar( driverBuffer[n + 2] );
                    
                    if (driverBuffer[n + 2] == ' ') 
                        break;
                }
                
                uint8_t msgFailedToLoad[] = "... failed";
                
                print(msgFailedToLoad, sizeof(msgFailedToLoad));
                printLn();
                
            }
            
#endif
            
            continue;
        }
        
        continue;
    }
    
    fsSetDeviceLetter('x');
    fsClearWorkingDirectory();
    
#endif
    
    
    
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
