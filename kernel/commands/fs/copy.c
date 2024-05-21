#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/copy.h>

#define FILE_BUFFER_SIZE   1024


uint8_t msgFileCopied[]         = "File copied";
uint8_t msgSourceNotFound[]     = "File not found";
uint8_t msgCannotCopyFile[]     = "Cannot be copied";
uint8_t msgErrorCreatingFile[]  = "Cannot create file";

void functionCOPY(uint8_t* param, uint8_t param_length) {
    
    uint8_t sourceFilename[FILE_NAME_LENGTH];
    uint8_t destFilename[FILE_NAME_LENGTH];
    
    uint8_t sourceNameLength = 0;
    uint8_t destNameLength   = 0;
    
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
    
    if (sourceFileSize > FILE_BUFFER_SIZE) 
        sourceFileSize = FILE_BUFFER_SIZE;
    
    // Check if the destination exists
    if (fsFileExists(destFilename, destNameLength) != 0) {
        
        print(msgCannotCopyFile, sizeof(msgCannotCopyFile));
        printLn();
        
        return;
    }
    
    // Create destination file
    uint32_t fileAddress = fsFileCreate(destFilename, destNameLength, sourceFileSize);
    
    if (fileAddress == 0) {
        print(msgErrorCreatingFile, sizeof(msgErrorCreatingFile));
        printLn();
        
        return;
    }
    
    // Copy file contents
    uint8_t fileBuffer[FILE_BUFFER_SIZE];
    
    uint8_t fileIndex = fsFileOpen(sourceFilename, sourceNameLength);
    fsFileRead(fileIndex, fileBuffer, sourceFileSize);
    
    fsFileClose(fileIndex);
    
    // Transfer to new file
    fileIndex = fsFileOpen(destFilename, destNameLength);
    fsFileWrite(fileIndex, fileBuffer, sourceFileSize);
    
    fsFileClose(fileIndex);
    
    // Complete
    print(msgFileCopied, sizeof(msgFileCopied));
    printLn();
    
    return;
}

void registerCommandCOPY(void) {
    
    uint8_t copyCommandName[] = "copy";
    
    ConsoleRegisterCommand(copyCommandName, sizeof(copyCommandName), functionCOPY);
    
    return;
}
