#include <avr/io.h>
#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>


uint8_t fsFileDelete(uint8_t* name, uint8_t nameLength) {
    
    if (fsFileExists(name, nameLength) == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate = 5;
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    uint8_t clearByte = 0x00;
    
    // Delete following sectors allocated to this file
    for (uint32_t sector=1; sector < currentCapacity; sector++) {
        
        // Find an active file start byte
        if (fsGetDeviceHeaderByte( sector * SECTOR_SIZE ) != 0x55) 
            continue;
        
        uint8_t isFileFound = 0;
        
        // Check file name
        for (uint8_t i=0; i < nameLength; i++) {
            
            uint8_t nameByte = 0;
            
            fs_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE) + OFFSET_FILE_NAME + i, &nameByte);
            
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
        
        uint8_t isHeaderDeleted = 0;
        
		struct Bus bus;
		bus.write_waitstate = 5;
		bus.write_waitstate = 5;
        
        // Delete the file sectors
        for (uint32_t nextSector = sector; nextSector < currentCapacity; nextSector++) {
            
            // Get sector header byte
            uint8_t headerByte = 0x00;
            fs_read_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), &headerByte);
            
            // Delete file header sector
            if (headerByte == 0x55) {
                
                // Only delete a header once
                if (isHeaderDeleted == 1) 
                    return 1;
                
                fs_write_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
                isHeaderDeleted = 1;
                continue;
            }
            
            // Delete data sector
            if (headerByte == 0xff) {
                
                fs_write_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
                continue;
            }
            
            // Delete end sector
            if (headerByte == 0xaa) {
                
                fs_write_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), clearByte);
                
                return 1;
            }
            
            // Empty sector
            if (headerByte == 0x00) 
                break;
            
            continue;
        }
        
		return 1;
    }
    
    return 0;
}

