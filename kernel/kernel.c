#include <kernel/kernel.h>
#include <kernel/delay.h>

#define INT_VECT_0  254
#define INT_VECT_1  253
#define INT_VECT_2  251
#define INT_VECT_3  247
#define INT_VECT_4  239
#define INT_VECT_5  223
#define INT_VECT_6  191
#define INT_VECT_7  127

#define INTERRUPT_VECTOR_TABLE_SIZE  8
void defaultLandingFunction(uint8_t index) {}

void (*interrupt_vector_table[INTERRUPT_VECTOR_TABLE_SIZE])(uint8_t);

void kInit(void) {
    
    // Initiate console prompt
    uint8_t prompt[] = {fsGetDeviceRoot(), '>'};
    
    ConsoleSetPrompt(prompt, sizeof(prompt));
    
    // Initiate external storage
    fsFormat(0, FORMAT_CAPACITY_32K);
    
    
    
    
    //
    // Executable example
    /*
    uint8_t versionFileName[]  = "ver";
    
    uint8_t bufferTest[] = {0x89, 0x01,  9,   // Call to print a string to the display
                            0x89, 0x06,  0,   // High byte offset to data start address
                            0x89, 0x07, 13,   // Low byte offset to data start address
                            0xcc, 0x10,       // Run display routine
                            0xcc, 0x20,       // Return to the shell
                            
                            'V', 'e', 'r', 's', 'i', 'o', 'n', ' ',
                            
                            ' ',' ',' ',' ',' ','\0','\0','\0'
                            
                            };
    
    uint32_t versionFileAddress = fsFileCreate(versionFileName, sizeof(versionFileName), sizeof(bufferTest));
    
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
        
        bufferTest[21] = versionMajorStr[0];
        bufferTest[22] = versionMajorStr[1];
        bufferTest[23] = '.';
        currentChar = 24;
    } else {
        bufferTest[21] = versionMajorStr[0];
        bufferTest[22] = '.';
        currentChar = 23;
    }
    
    if (minLen == 2) {
        
        bufferTest[currentChar]   = versionMinorStr[0];
        bufferTest[currentChar+1] = versionMinorStr[1];
        bufferTest[currentChar+2] = '.';
        currentChar += 3;
    } else {
        bufferTest[currentChar] = versionMinorStr[0];
        bufferTest[currentChar+1] = '.';
        currentChar += 2;
    }
    
    if (patLen == 2) {
        
        bufferTest[currentChar]   = versionPatchStr[0];
        bufferTest[currentChar+1] = versionPatchStr[1];
    } else {
        bufferTest[currentChar] = versionPatchStr[0];
    }
    
    fsFileWrite(bufferTest, sizeof(bufferTest));
    fsFileClose();
    
    
    
    //
    // Create bin directory
    
    uint8_t directoryName[] = "bin";
    fsDirectoryCreate(directoryName, sizeof(directoryName));
    
    fsChangeWorkingDirectory(directoryName, sizeof(directoryName));
    
    
    
    //
    // Create drivers directory
    
    fsClearWorkingDirectory();
    
    uint8_t directorySubName[] = "drivers";
    fsDirectoryCreate(directorySubName, sizeof(directorySubName));
    
    fsChangeWorkingDirectory(directorySubName, sizeof(directorySubName));
    
    
    
    
    
    //
    // Network driver
    
    uint8_t driverNicFileName[]  = "nic";
    uint32_t nicFileAddress = fsFileCreate(driverNicFileName, sizeof(driverNicFileName), 30);
    
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
    
    */
    
    
    
    
    
    
    
    fsClearWorkingDirectory();
    
    //
    // Create devices directory
    
    fsClearWorkingDirectory();
    
    uint8_t devicesSubDirName[] = "devices";
    uint32_t devicesDirectoryAddress = fsDirectoryCreate(devicesSubDirName, sizeof(devicesSubDirName));
    
    fsChangeWorkingDirectory(devicesSubDirName, sizeof(devicesSubDirName));
    
    
    
    //
    // Create a file representation of devices on the bus
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
        
        struct FSAttribute attribDoc;
        attribDoc.executable = ' ';
        attribDoc.readable   = 'r';
        attribDoc.writeable  = ' ';
        attribDoc.type       = ' ';
        
        fsFileSetAttributes(fileAddress, &attribDoc);
        
        fsFileOpen(fileAddress);
        
        // Device details
        
        uint8_t bufferDoc[] = "int 0x00\naddr 0x00000\n\0";
        uint8_t addrString[2];
        
        // Int vector offset
        int_to_hex_string(devicePtr->device_id, addrString);
        bufferDoc[6] = addrString[0];
        bufferDoc[7] = addrString[1];
        // Hardware address
        bufferDoc[16] = ('0' + (PERIPHERAL_ADDRESS_BEGIN / PERIPHERAL_STRIDE)) + devicePtr->hardware_slot;
        
        fsFileWrite(bufferDoc, sizeof(bufferDoc));
        fsFileClose();
        
        continue;
    }
    
    struct FSAttribute attribDeviceDir;
    attribDeviceDir.executable = ' ';
    attribDeviceDir.readable   = 'r';
    attribDeviceDir.writeable  = ' ';
    attribDeviceDir.type       = 'd';
    
    fsFileSetAttributes(devicesDirectoryAddress, &attribDeviceDir);
    
    fsClearWorkingDirectory();
    
    uint8_t selectedDevice = 'x';
    
#ifndef _BOOT_SAFEMODE__
    
    // Boot from device
    uint8_t driversDirName[] = "drivers";
    uint8_t kconfigDirName[] = "kconfig";
    
    //
    // Check any active storage devices
    for (uint8_t i=0; i < NUMBER_OF_PERIPHERALS; i++) {
        
        uint8_t currentDevice = i + 'a';
        
        fsSetDeviceLetter(currentDevice);
        
        if (fsCheckDeviceReady() == 0) 
            continue;
        
        //
        // Check KCONFIG
        //
        
        uint32_t kconfigAddress = fsFileExists(kconfigDirName, sizeof(kconfigDirName)-1);
        
        // Do not boot from device without
        // a kconfig settings file
        if (kconfigAddress == 0) {
            
            fsSetDeviceLetter('x');
            
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
        
        selectedDevice = currentDevice;
        
        break;
    }
    
#endif
    
    
    uint8_t promptDevLetter[] = "X>";
    
    if (selectedDevice == 0) {
        
        fsSetDeviceLetter('x');
        
    } else {
        
        fsSetDeviceLetter(selectedDevice);
        uppercase(&selectedDevice);
        
        promptDevLetter[0] = selectedDevice;
    }
    
    ConsoleSetPrompt(promptDevLetter, sizeof(promptDevLetter));
    
    fsClearWorkingDirectory();
    
    return;
}

void kernelVectorTableInit(void) {
    
    for (uint8_t i=0; i < INTERRUPT_VECTOR_TABLE_SIZE; i++) 
        interrupt_vector_table[i] = defaultLandingFunction;
    
    return;
}

void _ISR_hardware_service_routine(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    uint8_t intVect = 0;
    
    bus_read_io(&bus, 0x00000, &intVect);
    
    switch(intVect) {
        
        case INT_VECT_0: interrupt_vector_table[0](0); break;
        case INT_VECT_1: interrupt_vector_table[1](1); break;
        case INT_VECT_2: interrupt_vector_table[2](2); break;
        case INT_VECT_3: interrupt_vector_table[3](3); break;
        case INT_VECT_4: interrupt_vector_table[4](4); break;
        case INT_VECT_5: interrupt_vector_table[5](5); break;
        case INT_VECT_6: interrupt_vector_table[6](6); break;
        case INT_VECT_7: interrupt_vector_table[7](7); break;
        
        default:
            break;
        
    }
    
    return;
}

