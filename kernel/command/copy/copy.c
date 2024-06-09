#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/copy/copy.h>

uint8_t fileCopySourceName[FILE_NAME_LENGTH];
uint8_t fileCopySourceNameLength = 0;

uint8_t fileCopySourceWorkingDir[FILE_NAME_LENGTH];
uint8_t fileCopySourceWorkingDirLength = 0;

uint32_t fileCopySourceSize = 0;
struct FSAttribute fileCopySourceAttrib;

uint32_t fileCopySourceCurrentDevice;


void functionCOPY(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgFileCopied[]         = "File copied";
    uint8_t msgSourceNotFound[]     = "File not found";
    uint8_t msgErrorCreatingFile[]  = "Cannot create file";
    uint8_t msgFileAccessError[]    = "File access error";
    uint8_t msgDestinationError[]   = "Destination error";
    uint8_t msgBadName[]            = "Invalid filename";
    uint8_t msgSourceCopied[]       = "File referenced";
    
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
    
    // Ignore directories
    struct FSAttribute attributes;
    fsGetFileAttributes(sourceFilename, sourceNameLength, &attributes);
    
    if (attributes.type == 'd') {
        
        print(msgSourceNotFound, sizeof(msgSourceNotFound));
        printLn();
        
        return;
    }
    
    // Check destination filename
    if (((destNameLength - 2) < 1) | 
        (destFilename[0] == ' ')) {
        
        // Get file size
        uint32_t fileSize = fsGetFileSize(sourceFilename, sourceNameLength);
        
        if (fileSize > 0) {
            
            // Get file attributes
            fsGetFileAttributes(sourceFilename, sourceNameLength, &fileCopySourceAttrib);
            
            fileCopySourceSize = fileSize;
            
            fileCopySourceCurrentDevice = fsGetDevice();
            
            // Remember current working directory
            fileCopySourceWorkingDirLength = fsGetWorkingDirectory(fileCopySourceWorkingDir);
            
            // Copy file reference
            
            for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
                fileCopySourceName[i] = ' ';
            
            for (uint8_t i=0; i < sourceNameLength; i++) 
                fileCopySourceName[i] = sourceFilename[i];
            
            fileCopySourceNameLength = sourceNameLength;
            
            print(msgSourceCopied, sizeof(msgSourceCopied));
            printLn();
            
        } else {
            
            print(msgBadName, sizeof(msgBadName));
            printLn();
            
        }
        
        return;
    }
    
    //
    // Copy the file
    //
    
    uint32_t fileAddress = fsFileExists(sourceFilename, sourceNameLength);
    
    // Check source file exists
    if (fileAddress == 0) {
        
        print(msgSourceNotFound, sizeof(msgSourceNotFound));
        printLn();
        
        return;
    }
    
    // Get source file size
    uint32_t sourceFileSize = fsGetFileSize(sourceFilename, sourceNameLength);
    
    uint32_t sourceDevice = fsGetDevice();
    
    uint32_t currentDevice     = 0;
    uint32_t destinationDevice = 0;
    
    if (sourceDevice == 0x00000) {
        
        currentDevice     = 0xff;
        destinationDevice = 0xff;
        
    } else {
        
        currentDevice     = (sourceDevice - 0x10000) / 0x10000;
        destinationDevice = currentDevice;
    }
    
    if (destFilename[0] == 'X') {
        
        // Destination is the local system root
        
        fsSetDeviceLetter('X');
        destinationDevice = 0xff;
        destNameIsSource = 1;
        
    } else {
        
        // Destination is a peripheral device
        if ((destFilename[0] >= 'a') & 
            (destFilename[0] <= 'z') & 
            (destFilename[1] == ':')) {
            
            destinationDevice = destFilename[0] - 'a';
            
            fsSetDeviceLetter( destinationDevice );
            
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
        
        fsSetDeviceLetter(currentDevice);
        return;
    }
    
    
    //
    // Destination figured, create the file
    //
    
    if (fsFileCreate(destFilename, destNameLength, sourceFileSize, ' ') == 0) {
        
        print(msgErrorCreatingFile, sizeof(msgErrorCreatingFile));
        printLn();
        
        fsSetDeviceLetter(currentDevice + 'A');
        
        return;
    }
    
    
    
    
    // Copy file contents
    struct FSAttribute attributeSrc;
    uint8_t fileBuffer[sourceFileSize];
    
    fsSetDeviceLetter(currentDevice + 'A');
    fsGetFileAttributes(sourceFilename, sourceNameLength, &attributeSrc);
    
    uint8_t fileIndex = fsFileOpen(sourceFilename, sourceNameLength);
    
    if (fileIndex == 0) {
        
        print(msgFileAccessError, sizeof(msgFileAccessError));
        printLn();
        
        fsSetDeviceLetter(currentDevice + 'A');
        return;
    }
    
    fsFileReadBin(fileIndex, fileBuffer, sourceFileSize);
    
    fsFileClose(fileIndex);
    
    // Transfer to new file
    fsSetDeviceLetter(destinationDevice + 'A');
    fsSetFileAttributes(destFilename, destNameLength, &attributeSrc);
    
    fileIndex = fsFileOpen(destFilename, destNameLength);
    
    if (fileIndex == 0) {
        
        print(msgFileAccessError, sizeof(msgFileAccessError));
        printLn();
        
    }
    
    fsFileWrite(fileIndex, fileBuffer, sourceFileSize);
    
    fsFileClose(fileIndex);
    
    
    // Complete
    fsSetDeviceLetter(currentDevice + 'A');
    
    print(msgFileCopied, sizeof(msgFileCopied));
    printLn();
    
    return;
}

void registerCommandCOPY(void) {
    
    uint8_t copyCommandName[] = "copy";
    
    ConsoleRegisterCommand(copyCommandName, sizeof(copyCommandName), functionCOPY);
    
    return;
}
