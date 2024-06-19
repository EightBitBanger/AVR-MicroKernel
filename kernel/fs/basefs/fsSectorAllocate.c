#include <kernel/kernel.h>

extern uint32_t fs_device_address;

uint32_t fsSectorAllocate(uint32_t size) {
    
    uint32_t freeSectorCount = 0;
    uint32_t fileTargetAddress = 0;
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    // Calculate sectors required to fit the file
    // TODO: This is slow, optimize this out
    uint32_t totalSectors=0;
	for (uint32_t i=0; i < size; i += (SECTOR_SIZE - 1)) 
		totalSectors++;
	
	// Minimum of one sector
	if (totalSectors == 0) 
		totalSectors = 1;
	
	// Find free sectors
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Find an empty sector
        if (fsSectorGetByte( sector * SECTOR_SIZE ) != 0x00) 
            continue;
        
        // Find next sectors for total file size
        for (uint32_t nextSector = sector; nextSector < currentCapacity; nextSector++) {
            
            if (fsSectorGetByte( nextSector * SECTOR_SIZE ) == 0x00) {
                
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
            fsSectorSetByte(fileTargetAddress + (i * SECTOR_SIZE), 0xff);
        
        // Mark the end of file sector
        fsSectorSetByte(fileTargetAddress + (totalSectors * SECTOR_SIZE), 0xaa);
        
		// Mark the first sector
        uint8_t fileStartbyte = 0x55; // File start byte is 0x55
		fsSectorSetByte(fileTargetAddress, fileStartbyte);
		
		// Blank the file name
		for (uint8_t i=0; i < 10; i++) 
            fsSectorSetByte(fileTargetAddress + i + OFFSET_FILE_NAME, ' ');
        
        // Set file size
        union Pointer sizePtr;
        sizePtr.address = size;
        
        for (uint8_t i=0; i < 4; i++) 
            fsSectorSetByte(fileTargetAddress + i + OFFSET_FILE_SIZE, sizePtr.byte_t[i]);
        
        return fileTargetAddress;
    }
    
    return 0;
}
