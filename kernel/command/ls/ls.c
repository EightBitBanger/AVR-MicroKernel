#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/ls/ls.h>

extern struct Bus fs_bus;


void functionLS(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgDirectoryError[]    = "Invalid directory";
    uint8_t msgDirectoryListing[]  = "<DIR>";
    uint8_t msgPressAnyKey[]       = "Press any key...";
    
    uint16_t numberOfFiles = 0;
    uint16_t numberOfDirs  = 0;
    
    uint8_t fileCount = 0;
    
    
    //
    // List working directory contents
    //
    
    if (fsCheckWorkingDirectory() == 1) {
        
        uint8_t workingDirectory[20];
        uint8_t workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
        
        // Check if the current working directory is valid
        uint32_t directoryAddress = fsFileExists(workingDirectory, workingDirectoryLength-1);
		if (directoryAddress == 0) {
            
            print(msgDirectoryError, sizeof(msgDirectoryError));
            printLn();
            
            return;
		}
		
		uint32_t directorySize = fsGetFileSize(workingDirectory, workingDirectoryLength-1);
        
        uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles(workingDirectory, workingDirectoryLength-1);
        
        if (numberOfFiles > 0) {
            
            uint8_t index = fsFileOpen(workingDirectory, workingDirectoryLength-1);
            
            uint8_t bufferDir[directorySize];
            fsFileReadBin(index, bufferDir, directorySize);
            
            fsFileClose(index);
            
            // List the directory contents
            for (uint8_t i=0; i < numberOfFiles; i++) {
                
                // Get file address offset
                union Pointer fileAddressPtr;
                for (uint8_t p=0; p < 4; p++) 
                    fileAddressPtr.byte_t[p] = bufferDir[ (i * 4) + p ];
                uint32_t fileAddress = fileAddressPtr.address;
                
                // Attributes
                uint8_t attributes[4] = {' ',' ',' ',' '};
                
                for (uint8_t a=0; a < 4; a++) 
                    attributes[a] = fsSectorGetByte(fileAddress + a + OFFSET_FILE_ATTRIBUTES);
                
                print(&attributes[0], 4);
                printSpace(1);
                
                // Name
                uint8_t filename[10];
                
                for (uint8_t n=0; n < 10; n++) 
                    filename[n] = fsSectorGetByte(fileAddress + n + 1);
                
                print(filename, sizeof(filename) + 1);
                printSpace(1);
                
                // Check is directory
                if (attributes[3] == 'd') {
                    
                    // Directory listing
                    
                    print(msgDirectoryListing, sizeof(msgDirectoryListing));
                    
                    numberOfDirs++;
                    
                } else {
                    
                    // File listing
                    
                    // Size in bytes
                    union Pointer ptr;
                    
                    for (uint8_t s=0; s < 4; s++) 
                        ptr.byte_t[s] = fsSectorGetByte(fileAddress + s + OFFSET_FILE_SIZE);
                    
                    uint8_t filesizeString[10];
                    
                    uint8_t len = int_to_string( ptr.address, filesizeString ) + 1;
                    
                    if (len > 6) 
                        len = 6;
                    
                    uint8_t offset = 6 - len;
                    
                    if (offset > 5) 
                        offset = 5;
                    
                    printSpace(offset);
                    print(filesizeString, len);
                    
                }
                
                printLn();
                
                if ((param[0] == '-') & (param[1] == 'p')) {
                    
                    if (fileCount > 1) {
                        
                        print(msgPressAnyKey, sizeof(msgPressAnyKey));
                        
                        ConsoleSetCursorPosition( sizeof(msgPressAnyKey) - 1 );
                        
                        uint8_t keypress = ConsoleWait();
                        
                        ConsoleSetCursorPosition(0);
                        
                        for (uint8_t a=0; a < sizeof(msgPressAnyKey); a++) 
                            printChar(' ');
                        
                        ConsoleSetCursorPosition(0);
                        
                        fileCount = 0;
                        
                        if (keypress == 'c') 
                            return;
                        
                        continue;
                    }
                    
                    fileCount++;
                }
                
                continue;
            }
            
        }
        
        return;
    }
    
    
    //
    // List device root contents
    //
    
    for (uint16_t i=0; i < 1024; i++) {
        
        uint32_t fileAddress = fsFileFind(i);
        
        if (fileAddress == 0) 
            break;
        
        // List the file
        
        // Check if the file is claimed by a directory
        uint8_t flagClaimed = 0;
		fs_read_byte(fileAddress + OFFSET_DIRECTORY_FLAG, &flagClaimed);
		
		if (flagClaimed != 0) 
            continue;
        
        // Attributes
        uint8_t attributes[4] = {' ',' ',' ',' '};
        
        for (uint8_t a=0; a < 4; a++) 
            fs_read_byte(fileAddress + a + OFFSET_FILE_ATTRIBUTES, &attributes[a]);
        
        print(&attributes[0], 4);
        printSpace(1);
        
        // Name
        uint8_t filename[10];
        for (uint8_t n=0; n < 10; n++) 
            filename[n] = ' ';
        
        for (uint8_t n=0; n < 10; n++) 
            fs_read_byte(fileAddress + n + 1, &filename[n]);
        
        print(filename, sizeof(filename) + 1);
        printSpace(1);
        
        // Check is directory
        if (attributes[3] == 'd') {
            
            // Directory listing
            
            print(msgDirectoryListing, sizeof(msgDirectoryListing));
            
            numberOfDirs++;
            
        } else {
            
            // File listing
            
            // Size in bytes
            union Pointer ptr;
            
            for (uint8_t s=0; s < 4; s++) 
                fs_read_byte(fileAddress + s + OFFSET_FILE_SIZE, &ptr.byte_t[s]);
            
            uint8_t filesizeString[10];
            
            uint8_t len = int_to_string( ptr.address, filesizeString ) + 1;
            
            if (len > 6) 
                len = 6;
            
            uint8_t offset = 6 - len;
            
            if (offset > 5) 
                offset = 5;
            
            printSpace(offset);
            print(filesizeString, len);
            
            numberOfFiles++;
            
        }
        
        printLn();
        
        if ((param[0] == '-') & (param[1] == 'p')) {
            
            if (fileCount > 1) {
                
                print(msgPressAnyKey, sizeof(msgPressAnyKey));
                
                ConsoleSetCursorPosition( sizeof(msgPressAnyKey) - 1 );
                
                uint8_t keypress = ConsoleWait();
                
                ConsoleSetCursorPosition(0);
                
                for (uint8_t a=0; a < sizeof(msgPressAnyKey); a++) 
                    printChar(' ');
                
                ConsoleSetCursorPosition(0);
                
                fileCount = 0;
                
                if (keypress == 'c') 
                    return;
                
                continue;
            }
            
            fileCount++;
        }
        
        continue;
    }
    
    return;
}

void registerCommandLS(void) {
    
    uint8_t lsCommandName[] = "ls";
    
    ConsoleRegisterCommand(lsCommandName, sizeof(lsCommandName), functionLS);
    
    return;
}
