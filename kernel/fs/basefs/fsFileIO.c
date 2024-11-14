#include <kernel/kernel.h>

uint32_t fileBeginAddress = 0;
uint32_t fileSize = 0;


uint8_t fsFileOpen(uint32_t fileAddress) {
    
    if (fileBeginAddress != 0) 
        return 0;
    
    // Check file header byte
    uint8_t headerByte;
    fs_read_byte(fileAddress, &headerByte);
    
    if (headerByte != 0x55) 
        return 0;
    
    // Get file size
    fileSize = fsFileGetSize(fileAddress);
    
    fileBeginAddress = fileAddress;
    
    return 1;
}

uint8_t fsFileClose(void) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    fileBeginAddress = 0;
    fileSize = 0;
    
    return 1;
}

uint8_t fsFileWrite(uint8_t* buffer, uint32_t size) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (FORMAT_SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = fileBeginAddress + FORMAT_SECTOR_SIZE + sector + 1;
        
        fs_write_byte(positionOffset, buffer[i]);
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    return 1;
}

uint8_t fsFileRead(uint8_t* buffer, uint32_t size) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    if (size > (fileSize + 1)) 
        size = fileSize + 1;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (FORMAT_SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = fileBeginAddress + FORMAT_SECTOR_SIZE + sector + 1;
        
        fs_read_byte(positionOffset, &buffer[i]);
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    return 1;
}

uint8_t fsFileReadText(uint8_t* buffer, uint32_t size) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    if (size > (fileSize + 1)) 
        size = fileSize + 1;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (FORMAT_SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = fileBeginAddress + FORMAT_SECTOR_SIZE + sector + 1;
        
        fs_read_byte(positionOffset, &buffer[i]);
        
        sectorCounter++;
        sector++;
        
        if (buffer[i] == '\0') 
            break;
        
        continue;
    }
    
    buffer[size] = '\0';
    
    return 1;
}
