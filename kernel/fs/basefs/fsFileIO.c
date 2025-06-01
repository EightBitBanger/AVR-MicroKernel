#include <kernel/fs/fs.h>

#define MAX_OPEN_FILES  16

uint32_t fileBeginAddress[MAX_OPEN_FILES];
uint32_t fileSize[MAX_OPEN_FILES];


int32_t fsFileOpen(uint32_t fileAddress) {
    
    uint8_t index = 0;
    for (uint8_t i=0; i < MAX_OPEN_FILES+1; i++) {
        
        // Max files
        if (index == MAX_OPEN_FILES) 
            return -1;
        
        if (fileBeginAddress[i] == 0) {
            
            index = i;
            break;
        }
    }
    
    if (fileBeginAddress[index] != 0) 
        return -1;
    
    // Check file header byte
    uint8_t headerByte;
    fs_read_byte(fileAddress, &headerByte);
    
    if (headerByte != 0x55) 
        return -1;
    
    // Get file size
    fileSize[index] = fsFileGetSize(fileAddress);
    
    fileBeginAddress[index] = fileAddress;
    
    return index;
}

uint8_t fsFileClose(int32_t index) {
    
    if (index < 0 || index > MAX_OPEN_FILES) 
        return 0;
    
    if (fileBeginAddress[index] == 0) 
        return 0;
    
    fileBeginAddress[index] = 0;
    fileSize[index] = 0;
    
    return 1;
}

uint8_t fsFileWrite(int32_t index, uint8_t* buffer, uint32_t size) {
    
    if (index < 0 || index > MAX_OPEN_FILES) 
        return 0;
    
    if (fileBeginAddress[index] == 0) 
        return 0;
    
    uint32_t fileSz = fsFileGetSize( fileBeginAddress[index] );
    if (fileSz < size) 
        return 0;
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (fs_sector_size - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = fileBeginAddress[index] + fs_sector_size + sector + 1;
        
        fs_write_byte(positionOffset, buffer[i]);
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    return 1;
}

uint8_t fsFileRead(int32_t index, uint8_t* buffer, uint32_t size) {
    
    if (index < 0 || index > MAX_OPEN_FILES) 
        return 0;
    
    if (fileBeginAddress[index] == 0) 
        return 0;
    
    uint32_t fileSz = fsFileGetSize( fileBeginAddress[index] );
    if (fileSz < size) 
        return 0;
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    if (size > (fileSize[index] + 1)) 
        size = fileSize[index] + 1;
    
    for (uint32_t i=0; i < size; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (fs_sector_size - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = fileBeginAddress[index] + fs_sector_size + sector + 1;
        
        fs_read_byte(positionOffset, &buffer[i]);
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    return 1;
}
