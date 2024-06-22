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
    
    /*
    
    uint8_t msgFileCopied[]         = "File copied";
    uint8_t msgSourceNotFound[]     = "File not found";
    //uint8_t msgErrorCreatingFile[]  = "Cannot create file";
    //uint8_t msgFileAccessError[]    = "File access error";
    uint8_t msgDestinationError[]   = "Destination error";
    //uint8_t msgBadName[]            = "Invalid filename";
    uint8_t msgDeviceNotReady[]     = "Device not ready";
    
    uint8_t sourceFilename[FILE_NAME_LENGTH];
    uint8_t destFilename[FILE_NAME_LENGTH];
    
    uint8_t sourceNameLength = 0;
    uint8_t destNameLength   = 0;
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) {
        sourceFilename[i] = ' ';
        destFilename[i]   = ' ';
    }
    
    // Get filenames from parameters
    uint8_t index = StringSplit(param, param_length, sourceFilename, &sourceNameLength, destFilename, &destNameLength, ' ');
    
    
    // Ignore directories as source
    struct FSAttribute attributes;
    fsFileGetAttributes(sourceFilename, sourceNameLength, &attributes);
    
    if (attributes.type == 'd') {
        
        print(msgSourceNotFound, sizeof(msgSourceNotFound));
        printLn();
        
        return;
    }
    
    // Check filenames
    if ((sourceFilename[0] == ' ') | (destFilename[0] == ' ')) 
        return;
    
    
    // Cannot copy a directory
    if (fsCheckIsDirectory(sourceFilename, sourceNameLength) == 1) {
        
        print(msgSourceNotFound, sizeof(msgSourceNotFound));
        printLn();
        
        return;
    }
    
    //
    // Copy file data
    
    // TODO Check working directory
    index = fsFileOpen(sourceFilename, sourceNameLength);
    
    if (index == 0) {
        
        print(msgSourceNotFound, sizeof(msgSourceNotFound));
        printLn();
        
        return;
    }
    
    struct FSAttribute attrib;
    fsFileGetAttributes(sourceFilename, sourceNameLength, &attrib);
    
    uint32_t fileSize = fsGetFileSize(sourceFilename, sourceNameLength);
    
    uint8_t fileBuffer[fileSize];
    
    fsFileReadBin(index, fileBuffer, fileSize);
    
    fsFileClose(index);
    
    
    
    
    
    //
    // Check destination is new local file
    
    if (destFilename[1] != ':') {
        
        // Check destination is a directory
        if (fsCheckIsDirectory(destFilename, destNameLength) == 1) {
            
            // Destination is the working directory
            
            // Check if file exists in directory
            if (fsDirectoryFileExists(destFilename, destNameLength) != 0) {
                
                print(msgDestinationError, sizeof(msgDestinationError));
                printLn();
                
                return;
            }
            
            fsSetWorkingDirectory(destFilename, destNameLength);
            
            // Copy the file to the directory
            uint32_t fileAddress = fsFileCreate(sourceFilename, sourceNameLength, fileSize, ' ');
            
            if (fileAddress == 0) {
                
                print(msgDestinationError,sizeof(msgDestinationError));
                printLn();
                
                return;
            }
            
            index = fsFileOpen(sourceFilename, sourceNameLength);
            
            if (index == 0) {
                
                print(msgDestinationError,sizeof(msgDestinationError));
                printLn();
                
                return;
            }
            
            fsFileWrite(index, fileBuffer, fileSize);
            
            fsFileClose(index);
            
            // Set file attributes
            fsSectorSetByte(fileAddress + OFFSET_FILE_ATTRIBUTES + 0, attrib.executable);
            fsSectorSetByte(fileAddress + OFFSET_FILE_ATTRIBUTES + 1, attrib.readable);
            fsSectorSetByte(fileAddress + OFFSET_FILE_ATTRIBUTES + 2, attrib.writeable);
            fsSectorSetByte(fileAddress + OFFSET_FILE_ATTRIBUTES + 3, attrib.type);
            
            print(msgFileCopied,sizeof(msgFileCopied));
            printLn();
            
            // Restore working directory if it is not the root
            fsClearWorkingDirectory();
            
            return;
        }
        
        
        // Check destination file already exists
        if (fsFileExists(destFilename, destNameLength) != 0) {
            
            print(msgDestinationError, sizeof(msgDestinationError));
            printLn();
            
            return;
        }
        
        // Copy the file to a new file on the root
        
        if (fsFileCreate(destFilename, destNameLength, fileSize, ' ') != 0) {
            
            // Copy file data
            index = fsFileOpen(destFilename, destNameLength);
            
            if (index == 0) {
                
                print(msgDestinationError,sizeof(msgDestinationError));
                printLn();
                
                return;
            }
            
            fsFileWrite(index, fileBuffer, fileSize);
            
            fsFileClose(index);
            
            fsFileSetAttributes(destFilename, destNameLength, &attrib);
            
            print(msgFileCopied,sizeof(msgFileCopied));
            printLn();
            
            return;
        }
        
        print(msgDestinationError,sizeof(msgDestinationError));
        printLn();
        
        return;
    }
    
    
    
    //
    // Check destination is on another device
    
    if (destFilename[1] == ':') {
        
        // Check device letter
        if (is_letter(&destFilename[0]) == 0) {
            
            print(msgDestinationError, sizeof(msgDestinationError));
            printLn();
            
            return;
        }
        
        // Check destination device is active
        if (fsCheckDeviceReady() == 0) {
            
            print(msgDeviceNotReady, sizeof(msgDeviceNotReady));
            printLn();
            
            return;
        }
        
        // Save current device and working directory
        uint8_t currentDeviceLetter = fsGetRootDirectory();
        
        uint8_t currentWorkingDirectory[20];
        uint8_t currentWorkingDirectoryLength = fsGetWorkingDirectory(currentWorkingDirectory);
        
        // Shift to the new directory and device
        fsSetDeviceLetter( destFilename[0] );
        fsClearWorkingDirectory();
        
        uint32_t fileAddress = fsFileCreate(sourceFilename, sourceNameLength, fileSize, ' ');
        
        if (fileAddress == 0) {
            
            print(msgDestinationError, sizeof(msgDestinationError));
            printLn();
            
            // Restore the old device and working address
            fsSetDeviceLetter(currentDeviceLetter);
            fsSetWorkingDirectory(currentWorkingDirectory, currentWorkingDirectoryLength);
            
            return;
        }
        
        // Set file attributes
        fsSectorSetByte(fileAddress + OFFSET_FILE_ATTRIBUTES + 0, attrib.executable);
        fsSectorSetByte(fileAddress + OFFSET_FILE_ATTRIBUTES + 1, attrib.readable);
        fsSectorSetByte(fileAddress + OFFSET_FILE_ATTRIBUTES + 2, attrib.writeable);
        fsSectorSetByte(fileAddress + OFFSET_FILE_ATTRIBUTES + 3, attrib.type);
        
        // Copy data to the file
        
        index = fsFileOpen(sourceFilename, sourceNameLength);
        
        if (index == 0) {
            
            print(msgDestinationError,sizeof(msgDestinationError));
            printLn();
            
            // Restore the old device and working address
            fsSetDeviceLetter(currentDeviceLetter);
            fsSetWorkingDirectory(currentWorkingDirectory, currentWorkingDirectoryLength);
            
            return;
        }
        
        fsFileWrite(index, fileBuffer, fileSize);
        
        fsFileClose(index);
        
        print(msgFileCopied, sizeof(msgFileCopied));
        printLn();
        
        // Restore the old device and working address
        fsSetDeviceLetter(currentDeviceLetter);
        fsSetWorkingDirectory(currentWorkingDirectory, currentWorkingDirectoryLength);
        
        return;
    }
    
    */
    
    return;
}

void registerCommandCOPY(void) {
    
    uint8_t copyCommandName[] = "copy";
    
    ConsoleRegisterCommand(copyCommandName, sizeof(copyCommandName), functionCOPY);
    
    return;
}
