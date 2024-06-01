#include <kernel/kernel.h>

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize, uint8_t subType) {
    
    if (fsFileExists(name, nameLength) != 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate = 5;
    
    uint32_t freeSectorCount = 0;
    uint32_t fileTargetAddress = 0;
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity() / SECTOR_SIZE;
    
    if (fileSize > (SECTOR_SIZE * 8)) 
        return 0;
    
    // Calculate sectors required to fit the file
    uint32_t totalSectors=0;
	for (uint32_t i=0; i < fileSize; i += (SECTOR_SIZE - 1)) 
		totalSectors++;
	
	// Minimum of one sector
	if (totalSectors == 0) 
		totalSectors = 1;
	
	// Get working directory
	uint8_t workingDirectory[20];
    uint8_t workingDirectoryLength = fsGetWorkingDirectory(workingDirectory);
    
    uint32_t directoryAddress = 0;
	
	if ((workingDirectoryLength > 0) & (workingDirectory[0] != ' ')) 
        directoryAddress = fsFileExists(workingDirectory, workingDirectoryLength-1);
	
	
	// Find free sectors
    for (uint32_t sector=1; sector < currentCapacity; sector++) {
        
        // Get sector header byte
        uint8_t headerByte=0;
        fs_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE), &headerByte);
        
        // Find an empty sector
        if (fsGetDeviceHeaderByte( sector * SECTOR_SIZE ) != 0x00) 
            continue;
        
        // Find next sectors for total file size
        for (uint32_t nextSector = sector; nextSector < currentCapacity; nextSector++) {
            
            // Get sector header byte
            fs_read_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), &headerByte);
            
            if (fsGetDeviceHeaderByte( nextSector * SECTOR_SIZE ) == 0x00) {
                
                // Check target reached
                if (freeSectorCount == totalSectors) {
                    
                    fileTargetAddress = currentDevice + (sector * SECTOR_SIZE);
                    
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
            fs_write_byte(&bus, fileTargetAddress + (i * SECTOR_SIZE), 0xff);
        
        // Mark the end of file sector
        fs_write_byte(&bus, fileTargetAddress + (totalSectors * SECTOR_SIZE), 0xaa);
        
		// Mark the first sector
        uint8_t fileStartbyte = 0x55; // File start byte is 0x55
		fs_write_byte(&bus, fileTargetAddress, fileStartbyte);
		
		// Blank the file name
		for (uint8_t i=0; i < 10; i++) 
            fs_write_byte( &bus,  fileTargetAddress + i + OFFSET_FILE_NAME, ' ' );
        
		// Write file name
		for (uint8_t i=0; i < nameLength; i++) 
            fs_write_byte( &bus, fileTargetAddress + i + OFFSET_FILE_NAME, name[i] );
        
        // Set file size
        union Pointer sizePtr;
        sizePtr.address = fileSize;
        
        for (uint8_t i=0; i < 4; i++) 
            fs_write_byte( &bus, fileTargetAddress + i + OFFSET_FILE_SIZE, sizePtr.byte_t[i] );
        
        // Write file attributes
        uint8_t attributes[4] = {' ', 'r', 'w', subType};
        for (uint8_t i=0; i < 4; i++) 
            fs_write_byte( &bus, fileTargetAddress + i + OFFSET_FILE_ATTRIBUTES, attributes[i] );
        
        // Zero the directory size
        union Pointer dirSzPtr;
        dirSzPtr.address = 0;
        
        for (uint8_t i=0; i < 4; i++) 
            fs_write_byte( &bus, fileTargetAddress + i + OFFSET_DIRECTORY_SIZE, dirSzPtr.byte_t[i] );
        
        // Check file claimed by a directory
        uint8_t flagClaimed = 0;
        
        if (directoryAddress != 0) {
            flagClaimed = 1;
            
            uint32_t numberOfFiles = fsDirectoryGetNumberOfFiles(workingDirectory, workingDirectoryLength-1);
            
            // Add file reference to the directory
            
            uint32_t directorySize = fsGetFileSize(workingDirectory, workingDirectoryLength-1);
            
            uint8_t index = fsFileOpen(workingDirectory, workingDirectoryLength-1);
            
            uint8_t bufferRefs[directorySize];
            
            for (uint8_t i=0; i < directorySize; i++) 
                bufferRefs[i] = ' ';
            
            fsFileReadBin(index, bufferRefs, directorySize);
            
            union Pointer fileAddressPtr;
            fileAddressPtr.address = fileTargetAddress;
            
            for (uint8_t p=0; p < 4; p++) 
                bufferRefs[(numberOfFiles * 4) + p] = fileAddressPtr.byte_t[p];
            
            // Increment the directory file counter
            numberOfFiles++;
            fsDirectorySetNumberOfFiles(workingDirectory, workingDirectoryLength-1, numberOfFiles);
            
            fsFileWrite(index, bufferRefs, directorySize);
            
            fsFileClose(index);
        }
        
        // FLAG Claimed by a directory
        fs_write_byte(&bus, fileTargetAddress + OFFSET_DIRECTORY_FLAG, flagClaimed);
		
        return fileTargetAddress;
    }
    
    return 0;
}
