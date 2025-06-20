#include <kernel/fs/fs.h>

uint32_t fsDeviceConstructAllocationTable(uint32_t addressBegin, uint32_t addressEnd, uint32_t sectorSize) {
    
    // Write the device identifier bytes
    uint8_t deviceID[] = DEVICE_IDENTIFIER;
    
    for (uint8_t i=0; i < sizeof(deviceID); i++) 
        fs_write_byte(addressBegin + i, deviceID[i]);
    
    // Default device type generic
    fs_write_byte(addressBegin + DEVICE_OFFSET_TYPE, FORMAT_DEVICE_GENERIC);
    
    // Device total capacity
    union Pointer deviceSize;
    deviceSize.address = addressEnd - addressBegin;
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(addressBegin + DEVICE_OFFSET_CAPACITY + i, deviceSize.byte_t[i]);
    
    // Specify the sector size
    union Pointer deviceSectorSize;
    deviceSectorSize.address = sectorSize;
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(addressBegin + DEVICE_OFFSET_SECT_SZ + i, deviceSectorSize.byte_t[i]);
    
    // Create the root directory
    uint8_t rootDirName[] = "root";
    
    // Allocate specifically on the root
    uint32_t currentWorkingDirectory = fsWorkingDirectoryGetAddress();
    fsWorkingDirectorySetAddress(0);
    
    uint32_t directoryAddress = fsDirectoryCreate(rootDirName, sizeof(rootDirName));
    fsWorkingDirectorySetAddress(currentWorkingDirectory);
    
    // Set the device root directory
    fsDeviceSetRootContextDirectory( directoryAddress );
    
    return directoryAddress;
}


uint8_t fsFormat(uint32_t addressBegin, uint32_t addressEnd) {
    uint8_t rootDevice = fsDeviceGetRootLetter();
    
    // No caching for formatting on a memory device
    if (rootDevice == 'X') 
       fs_set_type_MEM_nocache();
    
    uint32_t sectorCounter = 0;
    
    for (uint32_t sector = addressBegin + 1; sector <= addressEnd; sector++) {
        if (sectorCounter < (fs_sector_size - 1)) {
            fs_write_byte(sector, ' ');
            sectorCounter++;
            
        } else {
            
            fs_write_byte(sector, FORMAT_SECTOR_EMPTY);
            sectorCounter = 0;
        }
        continue;
    }
    
    fsDeviceSetRootLetter(rootDevice);
    fsDeviceConstructAllocationTable(addressBegin, addressEnd, fs_sector_size);
    return 1;
}



uint8_t fsFormatQuick(uint32_t addressBegin, uint32_t addressEnd) {
    uint8_t rootDevice = fsDeviceGetRootLetter();
    if (rootDevice == 'X') 
        fs_set_type_MEM_nocache();
    
    for (uint32_t sector = addressBegin; sector <= addressEnd; sector += fs_sector_size) {
        fs_write_byte(sector, FORMAT_SECTOR_EMPTY);
        continue;
    }
    
    fsDeviceConstructAllocationTable(addressBegin, addressEnd, fs_sector_size);
    fsDeviceSetRootLetter(rootDevice);
    return 1;
}
