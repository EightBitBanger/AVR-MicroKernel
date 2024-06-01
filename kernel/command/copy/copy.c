#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/copy/copy.h>

void functionCOPY(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgFileCopied[]         = "File copied";
    uint8_t msgSourceNotFound[]     = "File not found";
    uint8_t msgErrorCreatingFile[]  = "Cannot create file";
    uint8_t msgFileAccessError[]    = "File access error";
    uint8_t msgDestinationError[]   = "Destination error";
    uint8_t msgBadName[]            = "Invalid filename";
    
    uint8_t sourceFilename[FILE_NAME_LENGTH];
    uint8_t destFilename[FILE_NAME_LENGTH];
    
    uint8_t sourceNameLength = 0;
    uint8_t destNameLength   = 0;
    
    uint8_t destNameIsSource = 0;
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
        sourceFilename[i] = ' ';
        destFilename[i]   = ' ';
    }
    
    // Get filenames from parameters
    uint8_t filename_index = 0;
    uint8_t index = 0;
    
    for (uint8_t i=0; i < param_length; i++) {
        
        if (filename_index == 0) {
            
            sourceFilename[i] = param[i];
            
            if (param[i] == ' ') {
                
                filename_index = 1;
                
                sourceNameLength = i + 1;
                
                continue;
            }
            
        } else {
            
            destFilename[index] = param[i];
            
            destNameLength = index + 2;
            
            if (param[i] == ' ') {
                break;
            }
            
            index++;
            
        }
        
    }
    
    // Check filenames
    if (((destNameLength - 2) < 1) | 
        (destFilename[0] == ' ')) {
        
        print(msgBadName, sizeof(msgBadName));
        printLn();
        return;
    }
    
    //
    // Copy the file
    //
    
    uint32_t sourceFileAddress = fsFileExists(sourceFilename, sourceNameLength);
    
    // Check source file exists
    if (sourceFileAddress == 0) {
        
        print(msgSourceNotFound, sizeof(msgSourceNotFound));
        printLn();
        
        return;
    }
    
    // Get source file size
    uint32_t sourceFileSize = fsGetFileSize(sourceFilename, sourceNameLength);
    
    uint32_t sourceDevice = fsGetCurrentDevice();
    
    uint32_t currentDevice     = 0;
    uint32_t destinationDevice = 0;
    
    if (sourceDevice == 0x00000) {
        
        currentDevice     = 0xff;
        
        destinationDevice = 0xff;
        
    } else {
        
        currentDevice = (sourceDevice - 0x10000) / 0x10000;
        
        destinationDevice = currentDevice;
    }
    
    if (destFilename[0] == '/') {
        
        // Destination is the local system root
        
        fsSetCurrentDevice( 0xff );
        
        destinationDevice = 0xff;
        
        destNameIsSource = 1;
        
    } else {
        
        // Destination is a peripheral device
        if ((destFilename[0] >= 'a') & 
            (destFilename[0] <= 'z') & 
            (destFilename[1] == ':')) {
            
            destinationDevice = destFilename[0] - 'a';
            
            fsSetCurrentDevice( destinationDevice );
            
            destNameIsSource = 1;
        }
        
    }
    
    // Destination name is the source name
    if (destNameIsSource == 1) {
        
        for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            destFilename[i] = sourceFilename[i];
        
        destNameLength = sourceNameLength;
    }
    
    
    // Check file already exists on destination device
    if (fsFileExists(destFilename, destNameLength) != 0) {
        
        print(msgDestinationError, sizeof(msgDestinationError));
        printLn();
        
        fsSetCurrentDevice(currentDevice);
        return;
    }
    
    // Create the file
    if (fsFileCreate(destFilename, destNameLength, sourceFileSize, ' ') == 0) {
        
        print(msgErrorCreatingFile, sizeof(msgErrorCreatingFile));
        printLn();
        
        fsSetCurrentDevice(currentDevice);
        return;
    }
    
    
    
    
    // Copy file contents
    struct FSAttribute attributes;
    uint8_t fileBuffer[sourceFileSize];
    
    fsSetCurrentDevice(currentDevice);
    fsGetFileAttributes(sourceFilename, sourceNameLength, &attributes);
    
    uint8_t fileIndex = fsFileOpen(sourceFilename, sourceNameLength);
    
    if (fileIndex == 0) {
        
        print(msgFileAccessError, sizeof(msgFileAccessError));
        printLn();
        
        fsSetCurrentDevice(currentDevice);
        return;
    }
    
    fsFileReadBin(fileIndex, fileBuffer, sourceFileSize);
    
    fsFileClose(fileIndex);
    
    
    // Transfer to new file
    fsSetCurrentDevice(destinationDevice);
    fsSetFileAttributes(destFilename, destNameLength, &attributes);
    
    fileIndex = fsFileOpen(destFilename, destNameLength);
    
    if (fileIndex == 0) {
        
        print(msgFileAccessError, sizeof(msgFileAccessError));
        printLn();
        
    }
    
    fsFileWrite(fileIndex, fileBuffer, sourceFileSize);
    
    fsFileClose(fileIndex);
    
    
    // Complete
    fsSetCurrentDevice(currentDevice);
    
    print(msgFileCopied, sizeof(msgFileCopied));
    printLn();
    
    return;
}

void registerCommandCOPY(void) {
    
    uint8_t copyCommandName[] = "copy";
    
    ConsoleRegisterCommand(copyCommandName, sizeof(copyCommandName), functionCOPY);
    
    return;
}
