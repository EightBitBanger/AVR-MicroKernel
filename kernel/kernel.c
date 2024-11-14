#include <kernel/kernel.h>

#include <kernel/delay.h>


#define INTERRUPT_VECTOR_TABLE_SIZE  8

void (*interrupt_vector_table[INTERRUPT_VECTOR_TABLE_SIZE])();

struct Bus isr_bus;

void nullfunction(void) {return;}

void AssembleJoin(uint8_t* buffer, uint32_t begin_address, uint8_t* source, uint32_t length) {
    
    for (uint32_t i=0; i < length; i++) 
        buffer[begin_address + i] = source[i];
    
    return;
}




void kInit(void) {
    
    VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_KERNEL);
    
    // Initiate console prompt
    uint8_t prompt[] = {fsDeviceGetRoot(), '>'};
    
    ConsoleSetPrompt(prompt, sizeof(prompt));
    
    // Initiate memory storage
    fsFormat(0, FORMAT_CAPACITY_32K);
    
    fsWorkingDirectoryClear();
    
    
    // Find an active storage device
    uint8_t promptDevLetter[] = " >";
    uint8_t selectedDevice = 255;
    
    for (uint8_t i=0; i < NUMBER_OF_PERIPHERALS; i++) {
        
        uint8_t currentDevice = i + 'a';
        
        fsDeviceSetLetter(currentDevice);
        
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
    
    fsDeviceSetLetter('x');
    fsWorkingDirectoryClear();
    
    uint8_t directoryName[] = "test";
    uint32_t directoryAddress = fsDirectoryCreate(directoryName, sizeof(directoryName));
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), directoryAddress);
    
    fsWorkingDirectoryChange(directoryName, sizeof(directoryName));
    
    uint8_t directoryNameWtf[] = "wtf";
    uint32_t directoryAddressWtf = fsDirectoryCreate(directoryNameWtf, sizeof(directoryNameWtf));
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), directoryAddressWtf);
    
    
    fsWorkingDirectoryClear();
    
    
    
    
    
    
    
    
    
    {
    
    uint8_t filename[]  = "cd";
    
    uint8_t buffer[200];
    
    
    uint8_t start[] = {
        
        MOV, rAH, 0x3B, 
        
        INT, 0x13, 
        
        CMP, rBL, 0, 
        JE, 0x00, 0x00, 0x00, 0x20, 
        
        INT, 0x20
    };
    
    uint8_t printPath[] = {
        
        MOV, rAH, 0x3F, 
        INT, 0x13, 
        INT, 0x20
    };
    
    
    AssembleJoin(buffer, 0x0000, start, sizeof(start));
    AssembleJoin(buffer, 0x0020, printPath, sizeof(printPath));
    
    uint32_t fileSizeTotal = 0x0020 + sizeof(printPath);
    
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), fileSizeTotal);
    
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    fsFileOpen(fileAddress);
    
    fsFileWrite(buffer, fileSizeTotal);
    fsFileClose();
    
    }
    
    
    
    
    
    
    
    
    
    
    
    if (selectedDevice == 255) {
        
        fsDeviceSetLetter('x');
        
        promptDevLetter[0] = 'X';
        
        ConsoleSetPrompt(promptDevLetter, sizeof(promptDevLetter));
        
    } else {
        
        fsDeviceSetLetter(selectedDevice);
        uppercase(&selectedDevice);
        
        promptDevLetter[0] = selectedDevice;
        
        ConsoleSetPrompt(promptDevLetter, sizeof(promptDevLetter));
        
        
    }
    
    fsWorkingDirectoryClear();
    
    
    
    
    
    
    
    
    
    
    
    return;
    
    
    
    /*
    
    {
    
    uint8_t filename[]  = "ld";
    
    uint8_t buffer[200];
    
    
    uint8_t start[] = {
        
        MOV, rAH, 0x0A, 
        MOV, rCL, 0, 
        MOV, rCH, 20, 
        
        INT, 0x47, 
        
        CMP, rBL, 1, 
        JE, 0x00, 0x00, 0x00, 0x30, 
        
        JNE, 0x00, 0x00, 0x00, 0x40, 
        
        INT, 0x20
        
    };
    
    uint8_t printPass[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x50, 
        INT, 0x10, 
        INT, 0x20
    };
    
    uint8_t printError[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x60, 
        INT, 0x10, 
        INT, 0x20
    };
    
    
    uint8_t text_pass[]  = "Driver loaded\0";
    uint8_t text_error[] = "Error\0";
    
    AssembleJoin(buffer, 0x0000, start, sizeof(start));
    AssembleJoin(buffer, 0x0030, printPass, sizeof(printPass));
    AssembleJoin(buffer, 0x0040, printError, sizeof(printError));
    
    AssembleJoin(buffer, 0x0050, text_pass, sizeof(text_pass));
    AssembleJoin(buffer, 0x0060, text_error, sizeof(text_error));
    
    uint32_t fileSizeTotal = 0x0060 + sizeof(text_error);
    
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), fileSizeTotal);
    
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    fsFileOpen(fileAddress);
    
    fsFileWrite(buffer, fileSizeTotal);
    fsFileClose();
    
    }
    */
    
    
    
    
    
    
    
    
    
    
    /*  
    
    
    
    uint8_t filename[]  = "mkdir";
    
    uint8_t buffer[200];
    
    
    uint8_t start[] = {
        
        MOV, rAH, 0x39, 
        MOV, rCL, 0, 
        MOV, rCH, 80, 
        
        INT, 0x13, 
        
        CMP, rBL, 0, 
        JE, 0x00, 0x00, 0x00, 0x20, 
        
        INT, 0x20,
        
    };
    
    uint8_t printError[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x30, 
        INT, 0x10, 
        INT, 0x20
    };
    
    uint8_t text_error[] = "Error\0";
    
    AssembleJoin(buffer, 0x0000, start, sizeof(start));
    AssembleJoin(buffer, 0x0020, printError, sizeof(printError));
    
    AssembleJoin(buffer, 0x0030, text_error, sizeof(text_error));
    
    uint32_t fileSizeTotal = 0x0030 + sizeof(text_error);
    
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), fileSizeTotal);
    
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    fsFileOpen(fileAddress);
    
    fsFileWrite(buffer, fileSizeTotal);
    fsFileClose();
    
    */
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    /*
    
    {
    
    uint8_t filename[]  = "mk";
    
    uint8_t buffer[200];
    
    
    uint8_t start[] = {
        
        MOV, rAH, 0x3c, 
        MOV, rCL, 0, 
        MOV, rCH, 20, 
        
        INT, 0x13, 
        
        CMP, rBL, 0, 
        JE, 0x00, 0x00, 0x00, 0x20, 
        
        INT, 0x20
        
    };
    
    uint8_t printError[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x30, 
        INT, 0x10, 
        INT, 0x20
    };
    
    
    uint8_t text_error[] = "Error\0";
    
    
    AssembleJoin(buffer, 0x0000, start, sizeof(start));
    AssembleJoin(buffer, 0x0020, printError, sizeof(printError));
    
    AssembleJoin(buffer, 0x0030, text_error, sizeof(text_error));
    
    uint32_t fileSizeTotal = 0x0030 + sizeof(text_error);
    
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), fileSizeTotal);
    
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    fsFileOpen(fileAddress);
    
    fsFileWrite(buffer, fileSizeTotal);
    fsFileClose();
    
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    {
    
    uint8_t filename[]  = "rm";
    
    uint8_t buffer[200];
    
    
    uint8_t start[] = {
        
        MOV, rAH, 0x41, // Delete file
        INT, 0x13, 
        
        CMP, rBL, 1, 
        JE, 0x00, 0x00, 0x00, 0x20, 
        
        MOV, rAH, 0x3A, // Delete directory
        INT, 0x13, 
        
        CMP, rBL, 1, 
        JE, 0x00, 0x00, 0x00, 0x30, 
        
        JMP, 0x00, 0x00, 0x00, 0x40,
        
    };
    
    uint8_t printFile[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x50, 
        INT, 0x10, 
        INT, 0x20
    };
    
    uint8_t printDir[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x60, 
        INT, 0x10, 
        INT, 0x20
    };
    
    uint8_t printError[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x80, 
        INT, 0x10, 
        INT, 0x20
    };
    
    uint8_t text_file[]     = "File deleted\0";
    uint8_t text_dir[]      = "Directory deleted\0";
    uint8_t text_error[]    = "Not found\0";
    
    AssembleJoin(buffer, 0x0000, start, sizeof(start));
    AssembleJoin(buffer, 0x0020, printFile, sizeof(printFile));
    AssembleJoin(buffer, 0x0030, printDir, sizeof(printDir));
    AssembleJoin(buffer, 0x0040, printError, sizeof(printError));
    
    AssembleJoin(buffer, 0x0050, text_file, sizeof(text_file));
    AssembleJoin(buffer, 0x0060, text_dir, sizeof(text_dir));
    AssembleJoin(buffer, 0x0080, text_error, sizeof(text_error));
    
    uint32_t fileSizeTotal = 0x0080 + sizeof(text_error);
    
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), fileSizeTotal);
    
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    fsFileOpen(fileAddress);
    
    fsFileWrite(buffer, fileSizeTotal);
    fsFileClose();
    
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    // Clear screen
    //
    
    uint8_t filename[]  = "cls";
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), 20);
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    fsFileOpen(fileAddress);
    
    uint8_t buffer[] = {0x89, 0x01,  3,   // Call to clear the display
                        0xcc, 0x10,       // Run display routine
                        0xcc, 0x20,       // Return to the shell
                        };
    
    fsFileWrite(buffer, sizeof(buffer));
    fsFileClose();
    
    
    
    
    */
    
    
    /*
    
    // System version
    //
    
    uint8_t filename[]  = "ver";
    
    uint8_t buffer[] = {0x89, 0x01,  9,   // Call to print a string to the display
                        0x89, 0x06,  0,   // High byte offset to data start address
                        0x89, 0x07, 13,   // Low byte offset to data start address
                        0xcc, 0x10,       // Run display routine
                        0xcc, 0x20,       // Return to the shell
                        
                        'V', 'e', 'r', 's', 'i', 'o', 'n', ' ',
                        
                        ' ',' ',' ',' ',' ','\0','\0','\0'
                        
                        };
    
    uint32_t versionFileAddress = fsFileCreate(filename, sizeof(filename), sizeof(buffer));
    
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), versionFileAddress);
    
    struct FSAttribute attribTest;
    attribTest.executable = 'x';
    attribTest.readable   = 'r';
    attribTest.writeable  = 'w';
    attribTest.type       = ' ';
    
    fsFileSetAttributes(versionFileAddress, &attribTest);
    
    uint8_t versionMajorStr[4];
    uint8_t versionMinorStr[4];
    uint8_t versionPatchStr[4];
    
    uint8_t majLen = int_to_string( _KERNEL_VERSION_MAJOR__, versionMajorStr );
    uint8_t minLen = int_to_string( _KERNEL_VERSION_MINOR__, versionMinorStr );
    uint8_t patLen = int_to_string( _KERNEL_VERSION_PATCH__, versionPatchStr );
    
    fsFileOpen(versionFileAddress);
    
    uint8_t currentChar = 0;
    
    if (majLen == 2) {
        
        buffer[21] = versionMajorStr[0];
        buffer[22] = versionMajorStr[1];
        buffer[23] = '.';
        currentChar = 24;
    } else {
        buffer[21] = versionMajorStr[0];
        buffer[22] = '.';
        currentChar = 23;
    }
    
    if (minLen == 2) {
        
        buffer[currentChar]   = versionMinorStr[0];
        buffer[currentChar+1] = versionMinorStr[1];
        buffer[currentChar+2] = '.';
        currentChar += 3;
    } else {
        buffer[currentChar] = versionMinorStr[0];
        buffer[currentChar+1] = '.';
        currentChar += 2;
    }
    
    if (patLen == 2) {
        
        buffer[currentChar]   = versionPatchStr[0];
        buffer[currentChar+1] = versionPatchStr[1];
    } else {
        buffer[currentChar] = versionPatchStr[0];
    }
    
    fsFileWrite(buffer, sizeof(buffer));
    fsFileClose();
    
    
    */
    
    
    
    
    
    
    
    
    
    
    
    
    
    //
    // Network driver
    
    uint8_t driverNicFileName[]  = "nic";
    uint32_t nicFileAddress = fsFileCreate(driverNicFileName, sizeof(driverNicFileName), 30);
    
    fsDirectoryAddFileRef(fsWorkingDirectoryGetAddress(), nicFileAddress);
    
    fsFileOpen(nicFileAddress);
    uint8_t bufferDrvNic[] = "  network   $XXXXX";
    
    bufferDrvNic[0] = 'K';      // Marker bytes KD (kernel driver)
    bufferDrvNic[1] = 'D';
    
    // Next 10 bytes reserved for driver name
    
    bufferDrvNic[12] = '$';   // Marker byte '$'
    bufferDrvNic[13] = 0x14;  // Device ID
    bufferDrvNic[14] = 0x0a;  // Bus read wait-state
    bufferDrvNic[15] = 0x0a;  // Bus write wait-state
    bufferDrvNic[16] = 0x02;  // Bus interface type
    
    bufferDrvNic[17] = 0x00;  // Hardware address
    bufferDrvNic[18] = 0x00;  
    bufferDrvNic[19] = 0x00;  
    bufferDrvNic[20] = 0x00;  
    
    fsFileWrite(bufferDrvNic, sizeof(bufferDrvNic));
    fsFileClose();
    
    fsWorkingDirectoryClear();
    
    
    
    
    
    
    
    
    
    
    
    
    //
    // Create devices directory
    
    /*
    
    fsWorkingDirectoryClear();
    
    uint8_t devicesSubDirName[] = "dev";
    uint32_t devicesDirectoryAddress = fsDirectoryCreate(devicesSubDirName, sizeof(devicesSubDirName));
    
    fsDirectoryAddRef(
    
    //
    // Create a file representation of devices on the bus
    
    fsWorkingDirectoryChange(devicesSubDirName, sizeof(devicesSubDirName));
    
    uint8_t numberOfDevices = GetNumberOfDevices();
    
    for (uint8_t i=0; i < numberOfDevices; i++) {
        
        struct Device* devicePtr = GetHardwareDeviceByIndex(i);
        
        if (devicePtr->device_name[0] == ' ') 
            continue;
        
        if (devicePtr->device_id == 0) 
            continue;
        
        uint8_t fileName[DEVICE_NAME_LENGTH];
        uint8_t nameLength;
        for (nameLength=0; nameLength < DEVICE_NAME_LENGTH; nameLength++) 
            fileName[nameLength] = devicePtr->device_name[nameLength];
        
        uint32_t fileAddress = fsFileCreate(fileName, nameLength, 140);
        
        struct FSAttribute attrib;
        attrib.executable = ' ';
        attrib.readable   = 'r';
        attrib.writeable  = ' ';
        attrib.type       = ' ';
        
        fsFileSetAttributes(fileAddress, &attrib);
        
        fsFileOpen(fileAddress);
        
        // Device details
        
        uint8_t fileBuffer[] = "int 0x00\naddr 0x00000\n\0";
        uint8_t addrString[2];
        
        // Int vector offset
        int_to_hex_string(devicePtr->device_id, addrString);
        fileBuffer[6] = addrString[0];
        fileBuffer[7] = addrString[1];
        // Hardware address
        fileBuffer[16] = ('0' + (PERIPHERAL_ADDRESS_BEGIN / PERIPHERAL_STRIDE)) + devicePtr->hardware_slot;
        
        fsFileWrite(fileBuffer, sizeof(fileBuffer));
        fsFileClose();
        
        continue;
    }
    
    struct FSAttribute attribDeviceDir;
    attribDeviceDir.executable = ' ';
    attribDeviceDir.readable   = 'r';
    attribDeviceDir.writeable  = ' ';
    attribDeviceDir.type       = 'd';
    
    fsFileSetAttributes(devicesDirectoryAddress, &attribDeviceDir);
    */
    
    /*
    
    fsWorkingDirectoryClear();
    
    uint8_t selectedDevice = 'x';
    
    
#ifndef _BOOT_SAFEMODE__
    
    // Boot from device
    uint8_t driversDirName[] = "sys";
    uint8_t kconfigFileName[] = "kconfig";
    
    //
    // Check active storage devices
    
    for (uint8_t i=0; i < NUMBER_OF_PERIPHERALS; i++) {
        
        uint8_t currentDevice = i + 'a';
        
        fsDeviceSetLetter(currentDevice);
        
        if (fsDeviceCheckReady() == 0) 
            continue;
        
        
        
        
        
        
        uint8_t promptDevLetter[] = "X>";
        
        if (currentDevice == 'x') {
            
            fsDeviceSetLetter('x');
            
        } else {
            
            fsDeviceSetLetter(selectedDevice);
            uppercase(&selectedDevice);
            
            promptDevLetter[0] = selectedDevice;
        }
        
        ConsoleSetPrompt(promptDevLetter, sizeof(promptDevLetter));
        
        fsWorkingDirectoryClear();
        
        // Setup environment
        
        EnvironmentSetHomeChar( selectedDevice );
        
        uint8_t binPath[] = "bin";
        EnvironmentSetPathBin(binPath, sizeof(binPath));
        
        
        return;
        
        
        
        
        //
        // Check KCONFIG boot settings
        
        uint32_t kconfigAddress = fsFileExists(kconfigFileName, sizeof(kconfigFileName)-1);
        
        // Do not boot from device without
        // a kconfig settings file
        
        if (kconfigAddress == 0) {
            
            fsDeviceSetLetter('x');
            
            continue;
        }
        
        uint8_t kconfigBuffer[80];
        
        fsFileOpen(kconfigAddress);
        
        fsFileRead(kconfigBuffer, 30);
        
        fsFileClose();
        
        // Apply configuration settings from kconfig
        
        
        
        
        //
        // Locate drivers directory
        //
        
        uint32_t directoryAddress = fsDirectoryExists(driversDirName, sizeof(driversDirName)-1);
        
        // Directory does not exist
        if (directoryAddress == 0) 
            continue;
        
        fsWorkingDirectoryChange(driversDirName, sizeof(driversDirName));
        
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
            
            
            // Load the driver
            uint8_t libState = LoadLibrary(driverFilename, driverFilenameLength);
            
#ifdef _BOOT_DETAILS__
            
            if (libState == 2) {
                
                for (uint32_t n=0; n < 10; n++) {
                    
                    printChar( driverBuffer[n + 2] );
                    
                    if (driverBuffer[n + 2] == ' ') 
                        break;
                }
                
                printLn();
            }
            
            if (libState <= 0) {
                
                uint8_t msgFailedToLoad[] = "... failed";
                print(msgFailedToLoad, sizeof(msgFailedToLoad));
                
                printLn();
            }
            
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
        
        selectedDevice = currentDevice;
        
        VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_KERNEL);
        
        break;
    }
    
#endif
    
    uint8_t promptDevLetter[] = "X>";
    
    if (selectedDevice == 'x') {
        
        fsDeviceSetLetter('x');
        
    } else {
        
        fsDeviceSetLetter(selectedDevice);
        uppercase(&selectedDevice);
        
        promptDevLetter[0] = selectedDevice;
    }
    
    ConsoleSetPrompt(promptDevLetter, sizeof(promptDevLetter));
    
    fsWorkingDirectoryClear();
    
    // Setup environment
    
    EnvironmentSetHomeChar( selectedDevice );
    
    uint8_t binPath[] = "bin";
    EnvironmentSetPathBin(binPath, sizeof(binPath));
    */
    
    
    
    
    
    
    
    return;
}

void KernelVectorTableInit(void) {
    
    isr_bus.read_waitstate  = 2;
    isr_bus.write_waitstate = 4;
    
    for (uint8_t i=0; i < INTERRUPT_VECTOR_TABLE_SIZE; i++) 
        interrupt_vector_table[i] = nullfunction;
    
    return;
}

void SetInterruptVector(uint8_t index, void(*servicePtr)()) {
    
    interrupt_vector_table[ index ] = servicePtr;
    
    return;
}

void _ISR_hardware_service_routine(void) {
    
    uint8_t vect = 0;
    
    bus_read_io(&isr_bus, 0x00000, &vect);
    
    if (((vect >> 7) & 1) != 0) {interrupt_vector_table[0]();}
    if (((vect >> 6) & 1) != 0) {interrupt_vector_table[1]();}
    if (((vect >> 5) & 1) != 0) {interrupt_vector_table[2]();}
    if (((vect >> 4) & 1) != 0) {interrupt_vector_table[3]();}
    if (((vect >> 3) & 1) != 0) {interrupt_vector_table[4]();}
    if (((vect >> 2) & 1) != 0) {interrupt_vector_table[5]();}
    if (((vect >> 1) & 1) != 0) {interrupt_vector_table[6]();}
    if (((vect >> 0) & 1) != 0) {interrupt_vector_table[7]();}
    
    return;
}

