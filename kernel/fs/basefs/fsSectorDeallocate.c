#include <kernel/kernel.h>

extern uint32_t fs_device_address;

uint8_t fsSectorDeallocate(uint32_t address) {
    
    uint8_t isHeaderDeleted = 0;
    uint8_t clearByte = 0x00;
    
    // Get allocation size
    union Pointer allocSize;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(address + OFFSET_FILE_SIZE, &allocSize.byte_t[i]);
    
    // Delete the file sectors
    for (uint32_t sector = 0; sector < allocSize.address; sector+= SECTOR_SIZE) {
        
        // Get sector header byte
        uint8_t headerByte = 0x00;
        fs_read_byte(address + sector, &headerByte);
        
        // Delete file header sector
        if (headerByte == 0x55) {
            
            // Only delete a header once
            if (isHeaderDeleted == 1) 
                return 1;
            
            fs_write_byte(address + sector, clearByte);
            
            isHeaderDeleted = 1;
            continue;
        }
        
        // Delete data sector
        if (headerByte == 0xff) {
            
            fs_write_byte(address + sector, clearByte);
            
            continue;
        }
        
        // Delete end sector
        if (headerByte == 0xaa) {
            
            fs_write_byte(address + sector, clearByte);
            
            return 1;
        }
        
        // Empty sector
        if (headerByte == 0x00) 
            break;
        
        continue;
    }
    
    return 0;
}
