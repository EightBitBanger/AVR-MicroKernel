#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>


uint32_t fsGetFileSize(uint8_t* name, uint8_t nameLength) {
    
    uint32_t fileAddress = fsFileExists(name, nameLength);
    if (fileAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    // Read file size
    union Pointer fileSize;
    
    for (uint8_t i=0; i < 4; i++) 
        bus_read_byte( &bus, fileAddress + OFFSET_FILE_SIZE + i, &fileSize.byte_t[i]);
    
    return fileSize.address;
    
    /*
    
    
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
        
        // Read file size
        union Pointer fileSize;
        
        for (uint8_t i=0; i < 4; i++) 
            bus_read_byte( &bus, currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_SIZE + i, &fileSize.byte_t[i]);
        
		return fileSize.address;
    }
    */
    
    return 0;
}

