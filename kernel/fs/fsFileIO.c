#include <kernel/kernel.h>

uint32_t fileBeginAddress = 0;
uint32_t fileSize = 0;

uint32_t fileSeek = 0;


uint8_t fsFileOpen(uint8_t* name, uint8_t nameLength) {
    
    fileBeginAddress = fsFileExists(name, nameLength);
    
    if (fileBeginAddress != 0) {
        
        fileSize = fsGetFileSize(name, nameLength) + 1;
        
        fileSeek = 0;
        
        return 1;
    }
    
    fileSize = 0;
    return 0;
}

uint8_t fsFileSeekGet(void) {
    return fileSeek;
}

uint8_t fsFileSeekSet(uint8_t position) {
    if (position > fileSize) 
        return 0;
    fileSeek = position;
    return 1;
}

uint8_t fsFileClose(uint8_t index) {
    
    if (fileBeginAddress != 0) {
        fileBeginAddress = 0;
        fileSize = 0;
        return 1;
    }
    
    return 0;
}

uint8_t fsFileWrite(uint8_t index, uint8_t* buffer, uint32_t length) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 5;
    bus.write_waitstate = 5;
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    if (length > (fileSize + 1)) 
        return 0;
    
    for (uint32_t i=0; i < length; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = fileBeginAddress + SECTOR_SIZE + sector + 1;
        
        fs_write_byte( &bus, positionOffset, buffer[i] );
        
        sectorCounter++;
        sector++;
        
        //if (buffer[i] == '\0') 
        //    break;
        
        continue;
    }
    
    return 1;
}

uint8_t fsFileReadText(uint8_t index, uint8_t* buffer, uint32_t length) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 5;
    bus.write_waitstate = 5;
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    if (length > (fileSize + 1)) 
        return 0;
    
    for (uint32_t i=0; i < length; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = fileBeginAddress + SECTOR_SIZE + sector + 1;
        
        fs_read_byte( &bus, positionOffset, &buffer[i] );
        
        sectorCounter++;
        sector++;
        
        if (buffer[i] == '\0') 
            break;
        
        continue;
    }
    
    buffer[length] = '\0';
    
    return ' ';
}


uint8_t fsFileReadBin(uint8_t index, uint8_t* buffer, uint32_t length) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 5;
    bus.write_waitstate = 5;
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    if (length > (fileSize + 1)) 
        return 0;
    
    for (uint32_t i=0; i < length; i++) {
        
        // Skip the sector marker byte
        if (sectorCounter == (SECTOR_SIZE - 1)) {
            sectorCounter=0;
            sector++;
        }
        
        uint32_t positionOffset = fileBeginAddress + SECTOR_SIZE + sector + 1;
        
        fs_read_byte( &bus, positionOffset, &buffer[i] );
        
        sectorCounter++;
        sector++;
        
        continue;
    }
    
    return ' ';
}
