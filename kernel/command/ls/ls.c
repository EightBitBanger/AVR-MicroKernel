#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/command/ls/ls.h>

void functionLS(uint8_t* param, uint8_t param_length) {
    
    struct Bus bus;
    bus.read_waitstate  = 5;
    bus.write_waitstate = 5;
    
    uint8_t fileCount = 0;
    
    uint16_t numberOfFiles = 0;
    uint16_t numberOfDirs  = 0;
    
    
    //
    // List directory contents
    //
    
    uint8_t workingDir[10];
    fsWorkingDirectoryGet(workingDir);
    uint8_t workingDirectoryLength = fsWorkingDirectoryGetLength();
    
    if ((workingDirectoryLength > 0) & (workingDir[0] != ' ')) {
        
        uint8_t workingDirectory[20];
        uint8_t workingDirectoryLength = fsWorkingDirectoryGet(workingDirectory);
        
        // Check if the current working directory is valid
        uint32_t fileAddress = fsFileExists(workingDirectory, workingDirectoryLength-1);
		if (fileAddress == 0) {
            
            uint8_t msgDirectoryError[] = "Invalid directory";
            
            print(msgDirectoryError, sizeof(msgDirectoryError));
            printLn();
            
            return;
		}
		
        // Get number of items in the directory
        
        union Pointer directorySize;
        for (uint8_t i=0; i < 4; i++) 
            fs_read_byte( &bus, fileAddress + i + OFFSET_DIRECTORY_SIZE, &directorySize.byte_t[i] );
        
        //directorySize.address;
        
        // Print the contents
        //for (uint16_t i=0; i < ) {
            
            uint8_t msgDURRR[] = "  items";
            
            msgDURRR[0] = directorySize.address + '0';
            
            print(msgDURRR, sizeof(msgDURRR));
            printLn();
        //}
        
        return;
    }
    
    
    //
    // List device root contents
    //
    
    for (uint32_t i=0; i < 0xffffffff; i++) {
        
        uint32_t fileAddress = fsFileFind(i);
        
        if (fileAddress == 0) 
            break;
        
        // List the file
        
        // Check if the file is claimed by a directory
        uint8_t flagClaimed = 0;
		fs_read_byte(&bus, fileAddress + OFFSET_DIRECTORY_FLAG, &flagClaimed);
		
		if (flagClaimed != 0) 
            continue;
        
        // Attributes
        uint8_t attributes[4] = {' ',' ',' ',' '};
        
        for (uint8_t a=0; a < 4; a++) 
            fs_read_byte( &bus, fileAddress + a + OFFSET_FILE_ATTRIBUTES, &attributes[a] );
        
        print(&attributes[0], 4);
        printSpace(1);
        
        // Name
        uint8_t filename[10];
        for (uint8_t n=0; n < 10; n++) 
            filename[n] = ' ';
        
        for (uint8_t n=0; n < 10; n++) 
            fs_read_byte( &bus, fileAddress + n + 1, &filename[n] );
        
        print(filename, sizeof(filename) + 1);
        printSpace(1);
        
        // Check is directory
        if (attributes[3] == 'd') {
            
            // Directory listing
            
            uint8_t msgDirectoryListing[] = "<DIR>";
            print(msgDirectoryListing, sizeof(msgDirectoryListing));
            
            numberOfDirs++;
            
        } else {
            
            // File listing
            
            
            // Size in bytes
            union Pointer ptr;
            
            for (uint8_t s=0; s < 4; s++) 
                fs_read_byte( &bus, fileAddress + s + OFFSET_FILE_SIZE, &ptr.byte_t[s] );
            
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
                
                uint8_t msgPressAnyKey[] = "Press any key...";
                print(msgPressAnyKey, sizeof(msgPressAnyKey));
                
                ConsoleSetCursorPosition( sizeof(msgPressAnyKey) - 1 );
                
                uint8_t keypress = ConsoleWait(0);
                
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
    
    // File count
    //uint16_t total = numberOfFiles + numberOfDirs;
    //printSpace(2);
    //printInt(total);
    //printSpace(1);
    
    //uint8_t msgOneFile[] = "File";
    //print(msgOneFile, sizeof(msgOneFile));
    
    //if (total != 1) 
    //    printChar('s');
    
    //printLn();
    
    return;
}

void registerCommandLS(void) {
    
    uint8_t lsCommandName[] = "ls";
    
    ConsoleRegisterCommand(lsCommandName, sizeof(lsCommandName), functionLS);
    
    return;
}
