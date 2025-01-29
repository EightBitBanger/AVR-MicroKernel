#include <kernel/kernel.h>

#include <kernel/command/boot/boot.h>

void DropFileSystemFunc(void);
void DropDeviceDrivers(void);

void functionBoot(uint8_t* param, uint8_t param_length) {
    
    if ((param[0] == '-') & 
        (param[1] == 'f') & 
        (param[2] == 'o') & 
        (param[3] == 'r') & 
        (param[4] == 'm') & 
        (param[5] == 'a') & 
        (param[6] == 't')) {
        
        uint8_t msgFormatting[] = "Formatting...";
        print(msgFormatting, sizeof(msgFormatting));
        printLn();
        
        uint32_t deviceCapacity = fsDeviceGetCapacity();
        
        if (deviceCapacity < FORMAT_CAPACITY_8K) 
            deviceCapacity = FORMAT_CAPACITY_8K;
        
        if (deviceCapacity > FORMAT_CAPACITY_32K) 
            deviceCapacity = FORMAT_CAPACITY_32K;
        
        fsFormat(0, deviceCapacity);
        
        fsWorkingDirectoryClear();
    }
    
    if ((param[0] == '-') & 
        (param[1] == 'b') & 
        (param[2] == 'i') & 
        (param[3] == 'n')) {
        
        uint8_t msgCreatingBins[] = "Creating binaries";
        print(msgCreatingBins, sizeof(msgCreatingBins));
        printLn();
        
        // Create 'bin' directory on the root
        
        uint8_t dirNameBin[] = "bin";
        uint32_t dirBinAddress = fsDirectoryCreate(dirNameBin, sizeof(dirNameBin));
        
        fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), dirBinAddress);
        
        fsWorkingDirectoryChange(dirNameBin, sizeof(dirNameBin));
        
        DropFileSystemFunc();
        
        fsWorkingDirectoryClear();
        
        
        uint8_t msgCreatingDrivers[] = "Creating drivers";
        print(msgCreatingDrivers, sizeof(msgCreatingDrivers));
        printLn();
        
        
        // Create 'sys' directory on the root
        
        uint8_t dirNameSys[] = "sys";
        uint32_t dirSysAddress = fsDirectoryCreate(dirNameSys, sizeof(dirNameSys));
        
        fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), dirSysAddress);
        
        fsWorkingDirectoryChange(dirNameSys, sizeof(dirNameSys));
        
        DropDeviceDrivers();
        
        fsWorkingDirectoryClear();
        
    }
    
    return;
}

void registerCommandBoot(void) {
    
    uint8_t bootCommandName[] = "boot";
    
    ConsoleRegisterCommand(bootCommandName, sizeof(bootCommandName), functionBoot);
    
    return;
}



void DropFileSystemFunc(void) {
    
    
    // MK - Create files
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
    
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    int32_t index = fsFileOpen(fileAddress);
    
    fsFileWrite(index, buffer, fileSizeTotal);
    fsFileClose(index);
    }
    
    
    // MKDIR - Create a directory
    {
    
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
    
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    int32_t index = fsFileOpen(fileAddress);
    
    fsFileWrite(index, buffer, fileSizeTotal);
    fsFileClose(index);
    }
    
    
    // RM - Remove a file or directory
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
    
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    int32_t index = fsFileOpen(fileAddress);
    
    fsFileWrite(index, buffer, fileSizeTotal);
    fsFileClose(index);
    }
    
    
    // CLS - Clear screen
    {
    
    uint8_t filename[]  = "cls";
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), 20);
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    int32_t index = fsFileOpen(fileAddress);
    
    uint8_t buffer[] = {0x89, 0x01,  3,   // Call to clear the display
                        0xcc, 0x10,       // Run display routine
                        0xcc, 0x20,       // Return to the shell
                        };
    
    fsFileWrite(index, buffer, sizeof(buffer));
    fsFileClose(index);
    }
    
    
    // VER - Show current system version
    {
    
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
    
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), versionFileAddress);
    
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
    
    int32_t index = fsFileOpen(versionFileAddress);
    
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
    
    fsFileWrite(index, buffer, sizeof(buffer));
    fsFileClose(index);
    }
    
    
    
    // Load library function
        
    {
    
    uint8_t filename[]  = "ld";
    
    uint8_t buffer[200];
    
    
    uint8_t start[] = {
        
        MOV, rAH, 0x0A, 
        MOV, rCL, 0, 
        MOV, rCH, 20, 
        
        INT, 0x47, 
        
        CMP, rBL, 2, 
        JE, 0x00, 0x00, 0x00, 0x40, 
        
        CMP, rBL, 1, 
        JE, 0x00, 0x00, 0x00, 0x30, 
        
        JNE, 0x00, 0x00, 0x00, 0x50, 
        
        INT, 0x20
        
    };
    
    uint8_t printPass[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x60, 
        INT, 0x10, 
        INT, 0x20
    };
    
    uint8_t printLoaded[] = {
        
        MOV, rAH, 0x09, 
        MOV, rDL, 0x00, 
        MOV, rDH, 0x70, 
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
    
    
    uint8_t text_pass[]   = "Driver loaded\0";
    uint8_t text_loaded[] = "Already loaded\0";
    uint8_t text_error[]  = "Error\0";
    
    AssembleJoin(buffer, 0x0000, start, sizeof(start));
    AssembleJoin(buffer, 0x0030, printPass, sizeof(printPass));
    AssembleJoin(buffer, 0x0040, printLoaded, sizeof(printLoaded));
    AssembleJoin(buffer, 0x0050, printError, sizeof(printError));
    
    
    AssembleJoin(buffer, 0x0060, text_pass, sizeof(text_pass));
    AssembleJoin(buffer, 0x0070, text_loaded, sizeof(text_loaded));
    AssembleJoin(buffer, 0x0080, text_error, sizeof(text_error));
    
    uint32_t fileSizeTotal = 0x0080 + sizeof(text_error);
    
    uint32_t fileAddress = fsFileCreate(filename, sizeof(filename), fileSizeTotal);
    
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), fileAddress);
    
    struct FSAttribute attrib = {'x', 'r', 'w', ' '};
    fsFileSetAttributes(fileAddress, &attrib);
    
    int32_t index = fsFileOpen(fileAddress);
    
    fsFileWrite(index, buffer, fileSizeTotal);
    fsFileClose(index);
    
    }
    
    return;
}



void DropDeviceDrivers(void) {
    
    // NIC - Network interface controller driver
    
    uint8_t driverNicFileName[]  = "nic";
    uint32_t nicFileAddress = fsFileCreate(driverNicFileName, sizeof(driverNicFileName), 30);
    
    fsDirectoryAddFile(fsWorkingDirectoryGetAddress(), nicFileAddress);
    
    int32_t index = fsFileOpen(nicFileAddress);
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
    
    fsFileWrite(index, bufferDrvNic, sizeof(bufferDrvNic));
    fsFileClose(index);
    
    return;
}
