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
    
    uint32_t directorySize = 0;
    
    uint8_t fileCount = 0;
    
    // Check device ready
    if (fsCheckDeviceReady() == 0) 
        return;
    
    uint8_t printPause(void) {
        
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
                    return 1;
            }
            
            fileCount++;
        }
        
        return 0;
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
        
        if (len > 6) 
            len = 6;
        
        uint8_t offset = 6 - len;
        
        if (offset > 5) 
            offset = 5;
        
        printSpace(offset);
        print(filesizeString, len);
        
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
        
        print(msgDirectoryListing, sizeof(msgDirectoryListing));
        
        return;
    }
    
    
    
    //
    // List working directory contents
    //
    
    if (fsCheckWorkingDirectory() == 1) {
        
        // Check if the current working directory is valid
        uint32_t directoryAddress = fsWorkingDirectoryGetAddress();
		
		if (directoryAddress == 0) {
            
            print(msgDirectoryError, sizeof(msgDirectoryError));
            printLn();
            
            return;
		}
		
		// Get file size
		directorySize = fsDirectoryGetSize(directoryAddress);
		
		// Get number of files
		numberOfFiles = fsDirectoryGetNumberOfFiles(directoryAddress);
		
        if (numberOfFiles > 0) {
            
            fsFileOpen(directoryAddress);
            
            uint8_t bufferDir[directorySize];
            fsFileRead(bufferDir, directorySize);
            
            fsFileClose();
             
            // List directories first
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
                
                numberOfDirs++;
                
                if (printPause() == 1) 
                    return;
                
                continue;
            }
            
            
            //
            // List files
            
            for (uint8_t i=0; i < numberOfFiles; i++) {
                
                // Get file address offset
                uint32_t fileAddress = fsDirectoryGetFile(directoryAddress, i);
                
                // Attributes
                struct FSAttribute attribute;
                fsFileGetAttributes(fileAddress, &attribute);
                
                // Check is directory
                if (attribute.type == 'd') 
                    continue;
                
                ListFile(fileAddress);
                
                printLn();
                
                continue;
            }
            
            numberOfFiles -= numberOfDirs;
            
        }
        
        return;
    }
    
    return;
}

void registerCommandLS(void) {
    
    uint8_t lsCommandName[] = "ls";
    
    ConsoleRegisterCommand(lsCommandName, sizeof(lsCommandName), functionLS);
    
    return;
}
