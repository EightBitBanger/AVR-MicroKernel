<<<<<<< HEAD
#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/ls/ls.h>

void ListFile(uint32_t fileAddress);
void ListDirectory(uint32_t fileAddress);


void functionLS(uint8_t* param, uint8_t param_length) {
    
    // Check device ready
    if (fsDeviceCheckReady() == 0) {
        
        uint8_t msgDeviceNotReady[]    = "Device not ready";
        
        print(msgDeviceNotReady, sizeof(msgDeviceNotReady));
        printLn();
        
        return;
    }
    
    
    // Check if the current working directory is valid
    uint32_t directoryAddress = fsWorkingDirectoryGetAddress();
    if (directoryAddress == 0) 
        return;
	
    for (uint16_t i=0; i < 0xffff; i++) {
        
        // Get file size
        uint32_t directorySize = fsDirectoryGetSize(directoryAddress);
        
        // Get number of files
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles(directoryAddress);
        
        int32_t index = fsFileOpen(directoryAddress);
        
        uint8_t buffer[directorySize];
        
        fsFileRead(index, buffer, directorySize);
        
        fsFileClose(index);
        
        // List sub-directories
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            // Get file address offset
            uint32_t fileAddress = fsDirectoryGetFile(directoryAddress, i);
            
            // Attributes
            struct FSAttribute attribute;
            fsFileGetAttributes(fileAddress, &attribute);
            
            // Check is directory
            if (attribute.type != 'd') 
                continue;
            
            ListDirectory(fileAddress);
            
            continue;
        }
        
        
        // List sub-files
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            // Get file address offset
            uint32_t fileAddress = fsDirectoryGetFile(directoryAddress, i);
            
            if (fileAddress == 0) 
                continue;
            
            // Attributes
            struct FSAttribute attribute;
            fsFileGetAttributes(fileAddress, &attribute);
            
            // Check is directory
            if (attribute.type == 'd') 
                continue;
            
            ListFile(fileAddress);
            
            continue;
        }
        
        
        //
        // Get next directory offset
        
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last directory block
        if (directoryAddress == 0) 
            return;
        
        continue;
    }
    
    return;
}

void registerCommandLS(void) {
    
    uint8_t lsCommandName[] = "ls";
    
    ConsoleRegisterCommand(lsCommandName, sizeof(lsCommandName), functionLS);
    
    return;
}


void ListFile(uint32_t fileAddress) {
    
    // Attributes
    struct FSAttribute attribute;
    fsFileGetAttributes(fileAddress, &attribute);
    
    print(&attribute.executable, 4);
    printSpace(1);
    
    // Name
    uint8_t filename[FILE_NAME_LENGTH];
    fsFileGetName(fileAddress, filename);
    
    print(filename, sizeof(filename) + 1);
    printSpace(1);
    
    // Size in bytes
    union Pointer ptr;
    
    for (uint8_t s=0; s < 4; s++) 
        fs_read_byte(fileAddress + s + FILE_OFFSET_SIZE, &ptr.byte_t[s]);
    
    uint8_t filesizeString[10];
    
    uint8_t len = int_to_string( ptr.address, filesizeString ) + 1;
    
    if (len > 7) 
        len = 7;
    
    uint8_t offset = 7 - len;
    
    if (offset > 6) 
        offset = 6;
    
    printSpace(offset);
    print(filesizeString, len);
    
    printLn();
    
    return;
}


void ListDirectory(uint32_t fileAddress) {
    
    // Attributes
    struct FSAttribute attribute;
    fsFileGetAttributes(fileAddress, &attribute);
    
    printChar(attribute.executable);
    printChar(attribute.readable);
    printChar(attribute.writeable);
    printSpace(1);
    
    // Name
    uint8_t filename[FILE_NAME_LENGTH];
    fsFileGetName(fileAddress, filename);
    
    print(filename, sizeof(filename) + 1);
    printSpace(2);
    
    uint8_t msgDirectoryListing[]  = "<DIR>";
    print(msgDirectoryListing, sizeof(msgDirectoryListing));
    
    printLn();
    
    return;
}
=======
#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/ls/ls.h>

void ListFile(uint32_t fileAddress);
void ListDirectory(uint32_t fileAddress);


void functionLS(uint8_t* param, uint8_t param_length) {
    
    // Check device ready
    if (fsDeviceCheckReady() == 0) {
        
        uint8_t msgDeviceNotReady[]    = "Device not ready";
        
        print(msgDeviceNotReady, sizeof(msgDeviceNotReady));
        printLn();
        
        return;
    }
    
    
    // Check if the current working directory is valid
    uint32_t directoryAddress = fsWorkingDirectoryGetAddress();
    if (directoryAddress == 0) 
        return;
	
    for (uint16_t i=0; i < 0xffff; i++) {
        
        // Get file size
        uint32_t directorySize = fsDirectoryGetSize(directoryAddress);
        
        // Get number of files
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles(directoryAddress);
        
        int32_t index = fsFileOpen(directoryAddress);
        
        uint8_t buffer[directorySize];
        
        fsFileRead(index, buffer, directorySize);
        
        fsFileClose(index);
        
        // List sub-directories
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            // Get file address offset
            uint32_t fileAddress = fsDirectoryGetFile(directoryAddress, i);
            
            // Attributes
            struct FSAttribute attribute;
            fsFileGetAttributes(fileAddress, &attribute);
            
            // Check is directory
            if (attribute.type != 'd') 
                continue;
            
            ListDirectory(fileAddress);
            
            continue;
        }
        
        
        // List sub-files
        
        for (uint8_t i=0; i < numberOfFiles; i++) {
            
            // Get file address offset
            uint32_t fileAddress = fsDirectoryGetFile(directoryAddress, i);
            
            if (fileAddress == 0) 
                continue;
            
            // Attributes
            struct FSAttribute attribute;
            fsFileGetAttributes(fileAddress, &attribute);
            
            // Check is directory
            if (attribute.type == 'd') 
                continue;
            
            ListFile(fileAddress);
            
            continue;
        }
        
        
        //
        // Get next directory offset
        
        directoryAddress = fsDirectoryGetNext(directoryAddress);
        
        // Check last directory block
        if (directoryAddress == 0) 
            return;
        
        continue;
    }
    
    return;
}

void registerCommandLS(void) {
    
    uint8_t lsCommandName[] = "ls";
    
    ConsoleRegisterCommand(lsCommandName, sizeof(lsCommandName), functionLS);
    
    return;
}


void ListFile(uint32_t fileAddress) {
    
    // Attributes
    struct FSAttribute attribute;
    fsFileGetAttributes(fileAddress, &attribute);
    
    print(&attribute.executable, 4);
    printSpace(1);
    
    // Name
    uint8_t filename[FILE_NAME_LENGTH];
    fsFileGetName(fileAddress, filename);
    
    print(filename, sizeof(filename) + 1);
    printSpace(1);
    
    // Size in bytes
    union Pointer ptr;
    
    for (uint8_t s=0; s < 4; s++) 
        fs_read_byte(fileAddress + s + FILE_OFFSET_SIZE, &ptr.byte_t[s]);
    
    uint8_t filesizeString[10];
    
    uint8_t len = int_to_string( ptr.address, filesizeString ) + 1;
    
    if (len > 7) 
        len = 7;
    
    uint8_t offset = 7 - len;
    
    if (offset > 6) 
        offset = 6;
    
    printSpace(offset);
    print(filesizeString, len);
    
    printLn();
    
    return;
}


void ListDirectory(uint32_t fileAddress) {
    
    // Attributes
    struct FSAttribute attribute;
    fsFileGetAttributes(fileAddress, &attribute);
    
    printChar(attribute.executable);
    printChar(attribute.readable);
    printChar(attribute.writeable);
    printSpace(1);
    
    // Name
    uint8_t filename[FILE_NAME_LENGTH];
    fsFileGetName(fileAddress, filename);
    
    print(filename, sizeof(filename) + 1);
    printSpace(2);
    
    uint8_t msgDirectoryListing[]  = "<DIR>";
    print(msgDirectoryListing, sizeof(msgDirectoryListing));
    
    printLn();
    
    return;
}
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
