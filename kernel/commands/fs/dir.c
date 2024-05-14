#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/commands/fs/dir.h>

void functionDIR(uint8_t* param, uint8_t param_length) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    for (uint32_t i=0; i < 0xffffffff; i++) {
        
        uint32_t fileAddress = fsFileFind(i);
        
        if (fileAddress == 0) 
            break;
        
        // List the file
        
        // Attributes
        uint8_t attributes[4] = {' ',' ',' ',' '};
        
        for (uint8_t a=0; a < 4; a++) 
            bus_read_io( &bus, fileAddress + a + OFFSET_FILE_ATTRIBUTES + 1, &attributes[a] );
        
        print(attributes, 4);
        printSpace(1);
        
        
        // Filename
        uint8_t filename[10] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
        
        for (uint8_t n=0; n < 10; n++) 
            bus_read_io( &bus, fileAddress + n + 1, &filename[n] );
        
        print(filename, sizeof(filename));
        printSpace(2);
        
        // File size
        union Pointer ptr;
        
        for (uint8_t s=0; s < 4; s++) 
            bus_read_io( &bus, fileAddress + s + OFFSET_FILE_SIZE, &ptr.byte_t[s] );
        
        uint8_t filesizeString[10];
        
        uint8_t len = int_to_string( ptr.address, filesizeString ) + 1;
        
        if (len > 6) 
            len = 6;
        
        uint8_t offset = 6 - len;
        
        if (offset > 5) 
            offset = 5;
        
        printSpace(offset);
        print(filesizeString, len);
        
        printLn();
        continue;
    }
    
    return;
}

void registerCommandDIR(void) {
    
    uint8_t dirCommandName[] = "dir";
    
    ConsoleRegisterCommand(dirCommandName, sizeof(dirCommandName), functionDIR);
    
    return;
}
