#include <kernel/fs/fs.h>

extern uint32_t __virtual_address_begin__;
extern uint32_t __virtual_address_end__;


uint32_t fsAllocate(uint32_t size) {
    
    uint32_t freeSectorCount = 0;
    uint32_t fileTargetAddress = 0;
    
    uint32_t deviceCapacity = fsDeviceGetCapacity() / fs_sector_size;
    
    // Calculate sectors required to fit the file
    uint32_t totalSectors = (size + (fs_sector_size - 2)) / (fs_sector_size - 1);
    
    // Find free sectors
    for (uint32_t sector = 0; sector < deviceCapacity; sector++) {
        
        uint32_t offset = __virtual_address_begin__ + (sector * fs_sector_size);
        
        // Check allocation range
        if (offset > __virtual_address_end__) {
            kThrow(HALT_BAD_ALLOCATION, offset);
        }
        
        // Find an empty sector
        uint8_t headerByte;
        fs_read_byte(offset, &headerByte);
        if (headerByte != FORMAT_SECTOR_EMPTY) 
            continue;
        
        // Check for enough contiguous sectors
        freeSectorCount = 0;
        for (uint32_t nextSector = sector; nextSector < deviceCapacity; nextSector++) {
            
            uint32_t offsetNext = __virtual_address_begin__ + (nextSector * fs_sector_size);
            
            // Check allocation range
            if (offsetNext > __virtual_address_end__) {
                kThrow(HALT_BAD_ALLOCATION, offsetNext);
            }
            
            uint8_t nextHeaderByte;
            fs_read_byte(offsetNext, &nextHeaderByte);
            
            if (nextHeaderByte == FORMAT_SECTOR_EMPTY) {
                freeSectorCount++;
                
                if (freeSectorCount == totalSectors) {
                    fileTargetAddress = offset;
                    break;
                }
                
            } else {
                freeSectorCount = 0;
                break;
            }
        }
        
        // File cannot fit into this empty space, continue seeking free space
        if (freeSectorCount != totalSectors)
            continue;
        
        // Mark the end of file sector
        fs_write_byte(fileTargetAddress + (totalSectors * fs_sector_size), FORMAT_SECTOR_FOOTER);
        
        // Mark the first sector
        fs_write_byte(fileTargetAddress, FORMAT_SECTOR_HEADER);
        
        // Mark following sectors as taken
        for (uint32_t i = 0; i < totalSectors - 1; i++) 
            fs_write_byte(fileTargetAddress + ((i + 1) * fs_sector_size), FORMAT_SECTOR_DATA);
        
        // Blank the file name
        for (uint8_t i = 0; i < FILE_NAME_LENGTH; i++) 
            fs_write_byte(fileTargetAddress + i + FILE_OFFSET_NAME, ' ');
        
        // Set file size
        union Pointer sizePtr;
        sizePtr.address = size;
        
        for (uint8_t i = 0; i < 4; i++) 
            fs_write_byte(fileTargetAddress + i + FILE_OFFSET_SIZE, sizePtr.byte_t[i]);
        
        return fileTargetAddress;
    }
    
    return 0;
}
