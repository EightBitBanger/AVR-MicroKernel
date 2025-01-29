#include <kernel/kernel.h>

#include <kernel/delay.h>


#define HARDWARE_INTERRUPT_TABLE_SIZE  8

void (*hardware_interrupt_table[HARDWARE_INTERRUPT_TABLE_SIZE])();

struct Bus isr_bus;

uint32_t dirProcAddress;

struct VirtualFileSystemInterface vfs;


void kInit(void) {
    
    kPrintVersion();
    
    VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_KERNEL);
    
    // Initiate console prompt
    uint8_t prompt[] = {fsDeviceGetRoot(), '>'};
    
    ConsoleSetPrompt(prompt, sizeof(prompt));
    
    // Initiate virtual file system
    vfs.open = vfsOpen;
    
    // Initiate memory storage
    fsDeviceSetRoot('x');
    fsFormat(0, FORMAT_CAPACITY_32K);
    
    fsWorkingDirectoryClear();
    
    
    // Create system directories
    
    uint8_t dirNameDev[] = "dev";
    uint32_t dirBinAddress = fsDirectoryCreate(dirNameDev, sizeof(dirNameDev));
    uint8_t dirNameProc[] = "proc";
    dirProcAddress = fsDirectoryCreate(dirNameProc, sizeof(dirNameProc));
    
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), dirBinAddress);
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), dirProcAddress);
    
    
    // Register devices on the bus
    
    fsWorkingDirectoryChange(dirNameDev, sizeof(dirNameDev));
    
    for (uint8_t index=0; index < NUMBER_OF_PERIPHERALS; index++) {
        
        struct Device* devPtr = GetDeviceByIndex( index );
        
        if (devPtr->device_id == 0) 
            continue;
        
        uint8_t length = 0;
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
            
            if (devPtr->device_name[i] == ' ') {
                
                length = i + 1;
                
                break;
            }
            
        }
        
        if (length == 0) 
            continue;
        
        uint32_t fileAddress = fsFileCreate(devPtr->device_name, length, 20);
        struct FSAttribute attrib = {' ','r',' ',' '};
        
        fsFileSetAttributes(fileAddress, &attrib);
        
        fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), fileAddress);
        
        
        continue;
    }
    
    fsWorkingDirectoryClear();
    
    
    
    
    
    // Find an active storage device
    uint8_t promptDevLetter[] = " >";
    uint8_t selectedDevice = 255;
    
    for (uint8_t i=0; i < NUMBER_OF_PERIPHERALS; i++) {
        
        uint8_t currentDevice = i + 'a';
        
        fsDeviceSetRoot(currentDevice);
        
        if (fsDeviceCheckReady() == 0) 
            continue;
        
        selectedDevice = currentDevice;
        
        // Setup environment
        
        // Primary root device
        EnvironmentSetHomeChar( selectedDevice );
        
        // Executable search path
        uint8_t binPath[] = "bin";
        EnvironmentSetPathBin(binPath, sizeof(binPath));
        
        break;
    }
    
    fsDeviceSetRoot('x');
    fsWorkingDirectoryClear();
    
    // Set prompt to an active storage device
    
    if (selectedDevice == 255) {
        
        fsDeviceSetRoot('x');
        
        promptDevLetter[0] = 'X';
        
        ConsoleSetPrompt(promptDevLetter, sizeof(promptDevLetter));
        
    } else {
        
        fsDeviceSetRoot(selectedDevice);
        uppercase(&selectedDevice);
        
        promptDevLetter[0] = selectedDevice;
        
        ConsoleSetPrompt(promptDevLetter, sizeof(promptDevLetter));
        
    }
    
    fsWorkingDirectoryClear();
    
    
    
    //
    // Load drivers
    
#ifndef _BOOT_SAFEMODE__
    
    uint8_t driversDirName[] = "sys";
    
    //
    // Locate drivers directory
    //
    
    uint32_t directoryAddress = fsDirectoryExists(driversDirName, sizeof(driversDirName)-1);
    
    // Directory does not exist
    if (directoryAddress == 0) 
        return;
    
    fsWorkingDirectoryChange(driversDirName, sizeof(driversDirName));
    
    uint32_t numberOfFiles = fsWorkingDirectoryGetFileCount();
    
    for (uint32_t f=0; f < numberOfFiles; f++) {
        
        uint32_t fileAddress = fsWorkingDirectoryFind(f);
        
        if (fileAddress == 0) 
            continue;
        
        uint32_t fileSize = fsFileGetSize(fileAddress);
        
        uint8_t driverBuffer[fileSize];
        
        int32_t index = fsFileOpen(fileAddress);
        
        fsFileRead(index, driverBuffer, fileSize);
        
        fsFileClose(index);
        
        uint8_t driverFilename[FILE_NAME_LENGTH];
        uint8_t driverFilenameLength=0;
        
        for (uint32_t n=0; n < FILE_NAME_LENGTH; n++) {
            
            fs_read_byte(fileAddress + FILE_OFFSET_NAME + n, &driverFilename[n]);
            
            if (driverFilename[n] == ' ') {
                driverFilenameLength = n;
                break;
            }
            
        }
        
        uint32_t checkFileAddress = fsFileExists(driverFilename, driverFilenameLength);
        
        // Load the driver
        int8_t libState = -32;
        if (checkFileAddress != 0) 
            libState = LoadLibrary(driverFilename, driverFilenameLength);
        
#ifdef _BOOT_DETAILS__
        
        // Driver is linked to a device on the bus
        if (libState == 2) {
            
            for (uint32_t n=0; n < FILE_NAME_LENGTH; n++) {
                
                printChar( driverBuffer[n + 2] );
                
                if (driverBuffer[n + 2] == ' ') 
                    break;
            }
            
            printLn();
        }
        
        // Driver was not loaded correctly or is corrupted
        if ((libState < 1) & (libState != -32)) {
            
            uint8_t msgFailedToLoad[] = "... failed";
            print(msgFailedToLoad, sizeof(msgFailedToLoad));
            
            printLn();
        }
        
#endif
        
        continue;
    }
    
#endif
    
    fsWorkingDirectoryClear();
    
    
    
    return;
}

void KernelVectorTableInit(void) {
    
    isr_bus.read_waitstate  = 1;
    
    for (uint8_t i=0; i < HARDWARE_INTERRUPT_TABLE_SIZE; i++) 
        hardware_interrupt_table[i] = nullfunction;
    
    return;
}

void SetInterruptVector(uint8_t index, void(*servicePtr)()) {
    
    hardware_interrupt_table[ index ] = servicePtr;
    
    return;
}

void _ISR_hardware_service_routine(void) {
    
    uint8_t vect = 0;
    
    bus_read_io(&isr_bus, 0x00000, &vect);
    
    if (((vect >> 7) & 1) != 0) {hardware_interrupt_table[0]();}
    if (((vect >> 6) & 1) != 0) {hardware_interrupt_table[1]();}
    if (((vect >> 5) & 1) != 0) {hardware_interrupt_table[2]();}
    if (((vect >> 4) & 1) != 0) {hardware_interrupt_table[3]();}
    if (((vect >> 3) & 1) != 0) {hardware_interrupt_table[4]();}
    if (((vect >> 2) & 1) != 0) {hardware_interrupt_table[5]();}
    if (((vect >> 1) & 1) != 0) {hardware_interrupt_table[6]();}
    if (((vect >> 0) & 1) != 0) {hardware_interrupt_table[7]();}
    
    return;
}


void nullfunction(void) {return;}
