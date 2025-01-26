#include <kernel/fs/fs.h>

uint32_t fsDeviceConstructAllocationTable(uint32_t addressBegin, uint32_t addressEnd) {
    
    // Write the device identifier bytes
    uint8_t deviceID[] = DEVICE_IDENTIFIER;
    
    for (uint8_t i=0; i < sizeof(deviceID); i++) 
        fs_write_byte(addressBegin + i, deviceID[i]);
    
    // Device total capacity
    union Pointer deviceSize;
    deviceSize.address = addressEnd - addressBegin;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(addressBegin + DEVICE_CAPACITY_OFFSET + i, deviceSize.byte_t[i]);
    
    // Create root directory
    uint8_t rootDirName[] = "root";
    uint8_t currentVirtualMode = VirtualAccessGetMode();
    VirtualAccessSetMode(VIRTUAL_ACCESS_MODE_KERNEL);
    
    uint32_t directoryAddress = fsDirectoryCreate(rootDirName, sizeof(rootDirName));
    
    fsDeviceSetRootDirectory( directoryAddress );
    
    VirtualAccessSetMode(currentVirtualMode);
    
    return directoryAddress;
}


uint8_t fsFormat(uint32_t addressBegin, uint32_t addressEnd) {
    
    uint32_t sectorCounter = 0;
    
    for (uint32_t sector = addressBegin + 1; sector <= addressEnd; sector++) {
        
        if (sectorCounter < (FORMAT_SECTOR_SIZE - 1)) {
            
            fs_write_byte(sector, ' ');
            sectorCounter++;
            
        } else {
            
            fs_write_byte(sector, FORMAT_SECTOR_EMPTY);
            sectorCounter = 0;
        }
        
        continue;
    }
    
    fsDeviceConstructAllocationTable(addressBegin, addressEnd);
    
    return 1;
}



uint8_t fsFormatQuick(uint32_t addressBegin, uint32_t addressEnd) {
    
    for (uint32_t sector = addressBegin; sector <= addressEnd; sector += FORMAT_SECTOR_SIZE) {
        
        fs_write_byte(sector, FORMAT_SECTOR_EMPTY);
        
        continue;
    }
    
    fsDeviceConstructAllocationTable(addressBegin, addressEnd);
    
    return 1;
}
