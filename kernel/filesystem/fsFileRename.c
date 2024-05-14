#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>


uint8_t fsFileRename(uint8_t* name, uint8_t nameLength, uint8_t* newName, uint8_t newNameLength) {
    
    if (fsFileExists(name, nameLength) == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate = 5;
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    // Check following sectors allocated to this file
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Find an active file start byte
        if (fsGetDeviceHeaderByte( sector * SECTOR_SIZE ) != 0x55) 
            continue;
        
        uint8_t isFileFound = 0;
        
        // Check file name
        for (uint8_t i=0; i < nameLength; i++) {
            
            uint8_t nameByte = 0;
            
            bus_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i, &nameByte);
            
            if (name[i] != nameByte) {
                isFileFound = 0;
                break;
            }
            
            isFileFound = 1;
            
            continue;
        }
        
        // Was the file located
        if (isFileFound == 0) 
            continue;
        
        // Replace the file name
        for (uint8_t i=0; i < newNameLength; i++) 
            bus_write_byte_eeprom(&bus, currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i, newName[i]);
        
		return 1;
    }
    
    return 0;
}

