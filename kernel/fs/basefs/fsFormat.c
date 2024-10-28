#include <kernel/kernel.h>

void fsFormatSetRootDirectory(uint32_t directoryAddress) {
    
    union Pointer rootDirPtr;
    rootDirPtr.address = directoryAddress;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(DEVICE_ROOT_DIR_OFFSET + i, rootDirPtr.byte_t[i]);
    
    return;
}


uint32_t fsFormatGetRootDirectory(void) {
    
    union Pointer rootDirPtr;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(DEVICE_ROOT_DIR_OFFSET + i, &rootDirPtr.byte_t[i]);
    
    return rootDirPtr.address;
}


uint8_t fsFotmatConstructAllocationTable(uint32_t addressBegin, uint32_t addressEnd) {
    
    fs_write_byte(addressBegin, 0x13 );
    fs_write_byte(addressBegin + 1, 'f' );
    fs_write_byte(addressBegin + 2, 's' );
    
    // Device total capacity
    union Pointer deviceSize;
    deviceSize.address = addressEnd - addressBegin;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(addressBegin + DEVICE_CAPACITY_OFFSET + i, deviceSize.byte_t[i]);
    
    // Set root directory
    uint8_t rootDirName[] = "root";
    
    uint32_t directoryAddress = fsDirectoryCreate(rootDirName, sizeof(rootDirName));
    
    fsFormatSetRootDirectory(directoryAddress);
    
    return 1;
}


uint8_t fsFormat(uint32_t addressBegin, uint32_t addressEnd) {
    
    uint32_t sectorCounter = 0;
    
    for (uint32_t sector = addressBegin + 1; sector <= addressEnd; sector++) {
        
        if (sectorCounter < (SECTOR_SIZE - 1)) {
            
            fs_write_byte(sector, ' ');
            sectorCounter++;
            
        } else {
            
            fs_write_byte(sector, 0x00);
            sectorCounter = 0;
        }
        
        continue;
    }
    
    fsFotmatConstructAllocationTable(addressBegin, addressEnd);
    
    fsSetWorkingDirectory( fsFormatGetRootDirectory() );
    
    return 1;
}


