#include <kernel/kernel.h>

uint8_t fsFree(uint32_t address) {
    
    uint8_t isHeaderDeleted = 0;
    uint8_t clearByte = 0x00;
    
    // Get allocation size
    union Pointer allocSize;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(address + FILE_OFFSET_SIZE, &allocSize.byte_t[i]);
    
    // Delete the file sectors
    for (uint32_t sector = 0; sector < allocSize.address; sector+= FORMAT_SECTOR_SIZE) {
        
        // Get sector header byte
        uint8_t headerByte = 0x00;
        fs_read_byte(address + sector, &headerByte);
        
        // Delete file header sector
        if (headerByte == FORMAT_SECTOR_HEADER) {
            
            // Only delete a header once
            if (isHeaderDeleted == 1) 
                return 1;
            
            fs_write_byte(address + sector, clearByte);
            
            isHeaderDeleted = 1;
            continue;
        }
        
        // Delete data sector
        if (headerByte == FORMAT_SECTOR_DATA) {
            
            fs_write_byte(address + sector, clearByte);
            
            continue;
        }
        
        // Delete end sector
        if (headerByte == FORMAT_SECTOR_FOOTER) {
            
            fs_write_byte(address + sector, clearByte);
            
            return 1;
        }
        
        // Empty sector
        if (headerByte == FORMAT_SECTOR_EMPTY) 
            break;
        
        continue;
    }
    
    return 0;
}
