#include <kernel/kernel.h>

uint32_t fsDirectoryGetFile(uint32_t directoryAddress, uint32_t index) {
    
	// Get file size
	uint32_t directorySize = fsDirectoryGetSize(directoryAddress);
    
    if (directorySize == 0) 
        return 0;
    
    if (fsFileOpen(directoryAddress) == 0) 
        return 0;
    
    uint8_t buffer[directorySize];
    fsFileRead(buffer, directorySize);
    
    fsFileClose();
    
    union Pointer fileAddressPtr;
    
    // Get file address offset
    for (uint8_t p=0; p < 4; p++) 
        fileAddressPtr.byte_t[p] = buffer[ (index * 4) + p ];
    
    return fileAddressPtr.address;
}
