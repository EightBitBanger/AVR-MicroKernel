#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/ls/ls.h>

extern struct Bus fs_bus;


void functionLS(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgDirectoryError[]    = "Invalid directory";
    uint8_t msgDirectoryListing[]  = "<DIR>";
    uint8_t msgPressAnyKey[]       = "Press any key...";
    uint8_t msgDeviceNotReady[]    = "Device not ready";
    
    uint16_t numberOfFiles = 0;
    uint16_t numberOfDirs  = 0;
    
    uint32_t directorySize = 0;
    
    uint8_t fileCount = 0;
    
    
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
    
    
    
    void GetAttributes(uint32_t fileAddress, uint8_t* attributes) {
        for (uint8_t a=0; a < 4; a++) 
            fs_read_byte(fileAddress + a + FILE_OFFSET_ATTRIBUTES, &attributes[a]);
        return;
    }
    
    void GetName(uint32_t fileAddress, uint8_t* filename) {
        for (uint8_t n=0; n < FILE_NAME_LENGTH; n++) 
            fs_read_byte(fileAddress + n + 1, &filename[n]);
        return;
    }
    
    
    
    void ListFile(uint32_t fileAddress) {
        
        // Attributes
        uint8_t attributes[4];
        GetAttributes(fileAddress, attributes);
        
        print(&attributes[0], 4);
        printSpace(1);
        
        // Name
        uint8_t filename[FILE_NAME_LENGTH];
        GetName(fileAddress, filename);
        
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
        uint8_t attributes[4];
        GetAttributes(fileAddress, attributes);
        
        printChar(attributes[0]);
        printChar(attributes[1]);
        printChar(attributes[2]);
        printSpace(1);
        
        // Name
        uint8_t filename[FILE_NAME_LENGTH];
        GetName(fileAddress, filename);
        
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
		union Pointer fileSizePtr;
		for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + i, &fileSizePtr.byte_t[i]);
        
		directorySize = fileSizePtr.address;
		
		// Get number of files
		union Pointer directorySizePtr;
		for (uint8_t i=0; i < 4; i++) 
            fs_read_byte(directoryAddress + DIRECTORY_OFFSET_SIZE + i, &directorySizePtr.byte_t[i]);
        
		numberOfFiles = directorySizePtr.address;
		
        if (numberOfFiles > 0) {
            
            fsFileOpen(directoryAddress);
            
            uint8_t bufferDir[directorySize];
            fsFileRead(bufferDir, directorySize);
            
            fsFileClose();
            
            union Pointer fileAddressPtr;
            
            // List directories first
            for (uint8_t i=0; i < numberOfFiles; i++) {
                
                // Get file address offset
                for (uint8_t p=0; p < 4; p++) 
                    fileAddressPtr.byte_t[p] = bufferDir[ (i * 4) + p ];
                
                uint32_t fileAddress = fileAddressPtr.address;
                
                // Attributes
                uint8_t attributes[4];
                GetAttributes(fileAddress, attributes);
                
                // Check is directory
                if (attributes[3] != 'd') 
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
                for (uint8_t p=0; p < 4; p++) 
                    fileAddressPtr.byte_t[p] = bufferDir[ (i * 4) + p ];
                
                uint32_t fileAddress = fileAddressPtr.address;
                
                // Attributes
                uint8_t attributes[4];
                GetAttributes(fileAddress, attributes);
                
                // Check is directory
                if (attributes[3] == 'd') 
                    continue;
                
                ListFile(fileAddress);
                
                printLn();
                
                continue;
            }
            
            numberOfFiles -= numberOfDirs;
            
        }
        
        return;
    }
    
    
    //
    // List device root contents
    //
    
    if (fsCheckDeviceReady() == 0) {
        
        print(msgDeviceNotReady, sizeof(msgDeviceNotReady));
        printLn();
        
        return;
    }
    
    uint8_t flagClaimed = 0;
    
    
    //
    // List directories
    
    for (uint32_t i=0; i < 1024; i++) {
        
        uint32_t fileAddress = fsFileFind(i);
        
        if (fileAddress == 0) 
            break;
        
        // List the file
        
        // Check if the file is claimed by a directory
        fs_read_byte(fileAddress + DIRECTORY_OFFSET_FLAG, &flagClaimed);
		
		if (flagClaimed != 0) 
            continue;
        
        // Check is directory
        uint8_t attributes[4];
        GetAttributes(fileAddress, attributes);
        
        if (attributes[3] != 'd') 
            continue;
        
        ListDirectory(fileAddress);
        
        numberOfDirs++;
        
        if (printPause() == 1) 
            return;
        
        continue;
    }
    
    
    //
    // List files
    
    for (uint32_t i=0; i < 1024; i++) {
        
        uint32_t fileAddress = fsFileFind(i);
        
        if (fileAddress == 0) 
            break;
        
        // List the file
        
        // Check if the file is claimed by a directory
        fs_read_byte(fileAddress + DIRECTORY_OFFSET_FLAG, &flagClaimed);
		
		if (flagClaimed != 0) 
            continue;
        
        // Check is directory
        uint8_t attributes[4];
        GetAttributes(fileAddress, attributes);
        
        if (attributes[3] == 'd') 
            continue;
        
        ListFile(fileAddress);
        
        numberOfFiles++;
        
        if (printPause() == 1) 
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
