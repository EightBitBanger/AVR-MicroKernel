#include <kernel/kernel.h>
#include <kernel/kalloc.h>

extern uint32_t VirtualAddressBegin;
extern uint32_t VirtualAddressEnd;


uint32_t fsAllocate(uint32_t size) {
    
    uint32_t freeSectorCount = 0;
    uint32_t fileTargetAddress = 0;
    
    uint32_t currentCapacity = fsDeviceGetCapacity() / FORMAT_SECTOR_SIZE;
    
    if (size < 1) 
        size = 1;
    
    // Calculate sectors required to fit the file
    uint32_t totalSectors=0;
	for (uint32_t i=0; i < size; i += (FORMAT_SECTOR_SIZE - 1)) 
		totalSectors++;
	
	// Minimum of one sector
	if (totalSectors == 0) 
		totalSectors = 1;
	
	// Find free sectors
    for (uint32_t sector=0; sector <= currentCapacity; sector++) {
        
        uint32_t offset = VirtualAddressBegin + (sector * FORMAT_SECTOR_SIZE);
        
        // Check allocation range
        if (offset >= VirtualAddressEnd) {
            
            kThrow(KERNEL_HALT_BAD_ALLOCATION, offset);
        }
        
        // Check out of memory range
        if (offset > kAllocGetTotal()) {
            
            kThrow(KERNEL_HALT_OUT_OF_MEMORY, offset);
        }
        
        // Find an empty sector
        uint8_t headerByte;
        fs_read_byte(offset, &headerByte);
        
        if (headerByte != 0x00) 
            continue;
        
        // Find next sectors for total file size
        for (uint32_t nextSector = sector; nextSector < currentCapacity; nextSector++) {
            
            uint32_t offsetNext = VirtualAddressBegin + (nextSector * FORMAT_SECTOR_SIZE);
            
            // Check allocation range
            if (offsetNext >= VirtualAddressEnd) {
                
                kThrow(KERNEL_HALT_BAD_ALLOCATION, offsetNext);
            }
            
            uint8_t nextHeaderByte;
            fs_read_byte(nextSector * FORMAT_SECTOR_SIZE, &nextHeaderByte);
            
            if (nextHeaderByte == 0) {
                
                // Check target reached
                if (freeSectorCount == totalSectors) {
                    
                    fileTargetAddress = offset;
                    
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
            fs_write_byte(fileTargetAddress + (i * FORMAT_SECTOR_SIZE), 0xff);
        
        // Mark the end of file sector
        fs_write_byte(fileTargetAddress + (totalSectors * FORMAT_SECTOR_SIZE), 0xaa);
        
		// Mark the first sector
        uint8_t fileStartbyte = FORMAT_SECTOR_HEADER;
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
