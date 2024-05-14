#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>

uint32_t fileBeginAddress = 0;


uint8_t fsFileOpen(uint8_t* name, uint8_t nameLength) {
    
    fileBeginAddress = fsFileExists(name, nameLength);
    
    if (fileBeginAddress != 0)
        return 1;
    
    return 0;
}

uint8_t fsFileClose(uint8_t index) {
    
    if (fileBeginAddress != 0) {
        fileBeginAddress = 0;
        return 1;
    }
    
    return 0;
}

uint8_t fsFileWrite(uint8_t index, uint8_t* buffer, uint8_t length) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    bus.write_waitstate = 5;
    
    for (uint32_t i=0; i < length; i++) 
        bus_write_io_eeprom(&bus, fileBeginAddress + SECTOR_SIZE + i + 1, buffer[i]);
    
    return 1;
}

uint8_t fsFileRead(uint8_t index, uint8_t* buffer, uint8_t length) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    bus.write_waitstate = 5;
    
    for (uint32_t i=0; i < length; i++) 
        bus_read_byte(&bus, fileBeginAddress + SECTOR_SIZE + i + 1, &buffer[i] );
    
    return ' ';
}

