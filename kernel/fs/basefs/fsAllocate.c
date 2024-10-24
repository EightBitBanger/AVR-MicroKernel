#include <kernel/kernel.h>

uint32_t fsAllocate(uint32_t size) {
    
    uint32_t freeSectorCount = 0;
    uint32_t fileTargetAddress = 0;
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    if (size < 0) 
        size = 1;
    
    // Calculate sectors required to fit the file
    uint32_t totalSectors=0;
	for (uint32_t i=0; i < size; i += (SECTOR_SIZE - 1)) 
		totalSectors++;
	
	// Minimum of one sector
	if (totalSectors == 0) 
		totalSectors = 1;
	
	// Find free sectors
    for (uint32_t sector=0; sector <= currentCapacity; sector++) {
        
        // Find an empty sector
        uint8_t headerByte;
        fs_read_byte(sector * SECTOR_SIZE, &headerByte);
        
        if (headerByte != 0x00) 
            continue;
        
        // Find next sectors for total file size
        for (uint32_t nextSector = sector; nextSector < currentCapacity; nextSector++) {
            
            uint8_t nextHeaderByte;
            fs_read_byte(nextSector * SECTOR_SIZE, &nextHeaderByte);
            
            if (nextHeaderByte == 0) {
                
                // Check target reached
                if (freeSectorCount == totalSectors) {
                    
                    fileTargetAddress = sector * SECTOR_SIZE;
                    
                    break;
                }
                
                freeSectorCount++;
                
                continue;
            }
            
            freeSectorCount = 0;
            
            break;
        }
        
        
		// File cannot fit into this empty space, continue seeking free space
		if (totalSectors != freeSectorCount) 
			continue;
		
        // Mark following sectors as taken
        for (uint32_t i = 0; i <= totalSectors; i++) 
            fs_write_byte(fileTargetAddress + (i * SECTOR_SIZE), 0xff);
        
        // Mark the end of file sector
        fs_write_byte(fileTargetAddress + (totalSectors * SECTOR_SIZE), 0xaa);
        
		// Mark the first sector
        uint8_t fileStartbyte = SECTOR_HEADER;
		fs_write_byte(fileTargetAddress, fileStartbyte);
		
		// Blank the file name
		for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
            fs_write_byte(fileTargetAddress + i + FILE_OFFSET_NAME, ' ');
        
        // Set file size
        union Pointer sizePtr;
        sizePtr.address = size;
        
        for (uint8_t i=0; i < 4; i++) 
            fs_write_byte(fileTargetAddress + i + FILE_OFFSET_SIZE, sizePtr.byte_t[i]);
        
        return fileTargetAddress;
    }
    
    return 0;
}
