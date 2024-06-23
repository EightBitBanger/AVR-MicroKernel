#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/copy/copy.h>

uint8_t fileCopySourceName[FILE_NAME_LENGTH];
uint8_t fileCopySourceNameLength = 0;

uint8_t fileCopySourceWorkingDir[FILE_NAME_LENGTH];
uint8_t fileCopySourceWorkingDirLength = 0;

uint32_t fileCopySourceSize = 0;
//struct FSAttribute fileCopySourceAttrib;

uint32_t fileCopySourceCurrentDevice;



void functionCOPY(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgFileCopied[]         = "File copied";
    uint8_t msgFileNotFound[]       = "File not found";
    //uint8_t msgFileAccessError[]    = "File access error";
    uint8_t msgDestinationError[]   = "Destination error";
    //uint8_t msgBadName[]            = "Invalid filename";
    //uint8_t msgDeviceNotReady[]     = "Device not ready";
    
    uint8_t sourceFilename[FILE_NAME_LENGTH];
    uint8_t destFilename[FILE_NAME_LENGTH];
    
    uint8_t sourceNameLength = 0;
    uint8_t destNameLength   = 0;
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
        sourceFilename[i] = ' ';
        destFilename[i]   = ' ';
    }
    
    // Get filenames from parameters
    StringSplit(param, param_length, sourceFilename, &sourceNameLength, destFilename, &destNameLength, ' ');
    
    // Copy source file
    uint32_t sourceAddress = fsFileExists(sourceFilename, sourceNameLength);
    
    // Get source attributes
    struct FSAttribute sourceAttrib;
    fsFileGetAttributes(sourceAddress, &sourceAttrib);
    
    // Check source is directory
    uint8_t sourceFileAttrib;
    fs_read_byte(sourceAddress + FILE_ATTRIBUTE_SPECIAL, &sourceFileAttrib);
    
    if ((sourceAddress == 0) | 
        (sourceFileAttrib == 'd')) {
        
        print(msgFileNotFound, sizeof(msgFileNotFound));
        printLn();
        
        return;
    }
    
	// Get file size
	union Pointer fileSizePtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(sourceAddress + FILE_OFFSET_SIZE + i, &fileSizePtr.byte_t[i]);
    
    fsFileOpen(sourceAddress);
    uint8_t sourceBuffer[fileSizePtr.address];
    
    for (uint8_t i=0; i < fileSizePtr.address; i++) 
        sourceBuffer[i] = ' ';
    
    fsFileRead(sourceBuffer, fileSizePtr.address);
    
    fsFileClose();
    
    // Remember current working directory
    uint8_t currentWorkingDirectory[20];
    
    for (uint8_t i=0; i < 20; i++) 
        currentWorkingDirectory[i] = ' ';
    
    // Check working directory
    uint8_t isInWorkingDirectory = fsCheckWorkingDirectory();
    
    uint8_t currentWorkingDirectoryLength = fsGetWorkingDirectory(currentWorkingDirectory);
    uint32_t currentWorkingDirectoryAddress = fsGetWorkingDirectoryAddress();
    
    uint32_t destinationFileAddress = fsFileExists(destFilename, destNameLength);
    
    if (destinationFileAddress != 0) {
        
        // Check if the destination is a directory
        uint8_t fileAttrib;
        fs_read_byte(destinationFileAddress + FILE_ATTRIBUTE_SPECIAL, &fileAttrib);
        
        if (fileAttrib == 'd') {
            
            // Copy to the destination directory
            
            fsSetWorkingDirectory(destFilename, destNameLength);
            fsSetWorkingDirectoryAddress(destinationFileAddress);
            
            // Check if the file already exists in the directory
            if (fsFileExists(sourceFilename, sourceNameLength) != 0) {
                
                print(msgDestinationError, sizeof(msgDestinationError));
                printLn();
                
                // Restore old working directory
                if (isInWorkingDirectory) {
                    fsSetWorkingDirectory(currentWorkingDirectory, currentWorkingDirectoryLength);
                    fsSetWorkingDirectoryAddress(currentWorkingDirectoryAddress);
                } else {
                    fsClearWorkingDirectory();
                }
                
                return;
            }
            
            uint32_t destFileAddress = fsFileCreate(sourceFilename, sourceNameLength, fileSizePtr.address);
            
            fsFileSetAttributes(destFileAddress, &sourceAttrib);
            
            fsFileOpen(destFileAddress);
            
            fsFileWrite(sourceBuffer, fileSizePtr.address);
            
            fsFileClose();
            
            print(msgFileCopied, sizeof(msgFileCopied));
            printLn();
            
            // Restore old working directory
            if (isInWorkingDirectory) {
                fsSetWorkingDirectory(currentWorkingDirectory, currentWorkingDirectoryLength);
                fsSetWorkingDirectoryAddress(currentWorkingDirectoryAddress);
            } else {
                fsClearWorkingDirectory();
            }
            
            return;
        }
        
        // Destination is a file
        
        print(msgDestinationError, sizeof(msgDestinationError));
        printLn();
        
        // Restore old working directory
        if (isInWorkingDirectory) {
            fsSetWorkingDirectory(currentWorkingDirectory, currentWorkingDirectoryLength);
            fsSetWorkingDirectoryAddress(currentWorkingDirectoryAddress);
        } else {
            fsClearWorkingDirectory();
        }
        
        return;
    }
    
    // Check copy to another device root
    if ((destFilename[1] == ':') | 
        (destFilename[2] == ' ')) {
        
        uint32_t currentDeviceAddress = fsGetDevice();
        uint8_t deviceLetter = fsGetDeviceRoot();
        fsSetDeviceLetter( destFilename[0] );
        fsClearWorkingDirectory();
        
        uint32_t destFileAddress = fsFileCreate(sourceFilename, sourceNameLength, fileSizePtr.address);
        fsFileSetAttributes(destFileAddress, &sourceAttrib);
        
        fsFileOpen(destFileAddress);
        fsFileWrite(sourceBuffer, fileSizePtr.address);
        fsFileClose();
        
        print(msgFileCopied, sizeof(msgFileCopied));
        printLn();
        
        fsSetDevice(currentDeviceAddress);
        fsSetDeviceLetter( deviceLetter );
        
        if (isInWorkingDirectory) {
            fsSetWorkingDirectory(currentWorkingDirectory, currentWorkingDirectoryLength);
            fsSetWorkingDirectoryAddress(currentWorkingDirectoryAddress);
        } else {
            fsClearWorkingDirectory();
        }
        
        return;
    }
    
    // Paste file into destination
    uint32_t destFileAddress = fsFileCreate(destFilename, destNameLength, fileSizePtr.address);
    
    fsFileSetAttributes(destFileAddress, &sourceAttrib);
    
    fsFileOpen(destFileAddress);
    
    fsFileWrite(sourceBuffer, fileSizePtr.address);
    
    fsFileClose();
    
    print(msgFileCopied, sizeof(msgFileCopied));
    printLn();
    
    
    // Restore old working directory
    if (isInWorkingDirectory) {
        fsSetWorkingDirectory(currentWorkingDirectory, currentWorkingDirectoryLength);
        fsSetWorkingDirectoryAddress(currentWorkingDirectoryAddress);
    } else {
        fsClearWorkingDirectory();
    }
    
    return;
}

void registerCommandCOPY(void) {
    
    uint8_t copyCommandName[] = "copy";
    
    ConsoleRegisterCommand(copyCommandName, sizeof(copyCommandName), functionCOPY);
    
    return;
}
