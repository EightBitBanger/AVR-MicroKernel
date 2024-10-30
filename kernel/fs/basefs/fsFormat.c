#include <kernel/kernel.h>

uint8_t fsFotmatConstructAllocationTable(uint32_t addressBegin, uint32_t addressEnd) {
    
    fs_write_byte(addressBegin, 0x13 );
    fs_write_byte(addressBegin + 1, 'f' );
    fs_write_byte(addressBegin + 2, 's' );
    
    // Device total capacity
    union Pointer deviceSize;
    deviceSize.address = addressEnd - addressBegin;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(addressBegin + DEVICE_CAPACITY_OFFSET + i, deviceSize.byte_t[i]);
    
    // Create root directory
    uint8_t rootDirName[] = "root";
    
    uint32_t directoryAddress = fsDirectoryCreate(rootDirName, sizeof(rootDirName));
    
    fsDeviceSetRootDirectory(directoryAddress);
    
    return 1;
}


uint8_t fsFormat(uint32_t addressBegin, uint32_t addressEnd) {
    
    uint32_t sectorCounter = 0;
    
    for (uint32_t sector = addressBegin + 1; sector <= addressEnd; sector++) {
        
        if (sectorCounter < (FORMAT_SECTOR_SIZE - 1)) {
            
            fs_write_byte(sector, ' ');
            sectorCounter++;
            
        } else {
            
            fs_write_byte(sector, 0x00);
            sectorCounter = 0;
        }
        
        continue;
    }
    
    fsFotmatConstructAllocationTable(addressBegin, addressEnd);
    
    fsSetWorkingDirectory( fsDeviceGetRootDirectory() );
    
    return 1;
}


