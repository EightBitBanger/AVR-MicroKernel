#include <kernel/fs/fs.h>
#include <kernel/bus/bus.h>
#include <string.h>

struct Bus fs_bus;

void fs_write_byte(uint32_t address, uint8_t data) {
    bus_write_memory(&fs_bus, address, data);
}

void fs_read_byte(uint32_t address, uint8_t* data) {
    bus_read_memory(&fs_bus, address, data);
}

void fsInit(void) {
    
    fs_bus.bus_type = 0;
    fs_bus.read_waitstate = 2;
    fs_bus.write_waitstate = 2;
    
}

uint8_t DeviceHeaderString[] = {0x13, 'f','s',' ',' ',' ',' ',' ',' ',' ',' '};


struct Partition fsDeviceOpen(uint32_t deviceAddress) {
    struct Partition part;
    part.block_address = deviceAddress;
    
    // Get device ID bytes
    uint8_t headerID[10];
    for (uint8_t i=0; i < sizeof(headerID); i++) {
        fs_read_byte(part.block_address + i, &headerID[i]);
    }
    
    part.block_size = fsDeviceGetSize(part);
    part.sector_size  = fsDeviceGetSectorSize(part);
    
    if (headerID[0] != 0x13 || headerID[1] != 'f' || headerID[2] != 's' ||
        part.block_size == 0 || part.sector_size == 0) {
        
        part.block_size = 0;
        part.sector_count = 0;
        part.sector_size = 0;
        return part;
    }
    
    part.sector_count = part.block_size / part.sector_size;
    return part;
}

uint32_t fsDeviceGetSize(struct Partition part) {
    uint8_t sizeBytes[4];
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(part.block_address + i + DEVICE_OFFSET_CAPACITY, &sizeBytes[i]);
    return *((uint32_t*)&sizeBytes[0]);
}

uint32_t fsDeviceGetSectorSize(struct Partition part) {
    uint8_t sizeBytes[4];
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(part.block_address + i + DEVICE_OFFSET_SECT_SZ, &sizeBytes[i]);
    return *((uint32_t*)&sizeBytes[0]);
}

DirectoryHandle fsDeviceGetRootDirectory(struct Partition part) {
    uint8_t ptrBytes[4];
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(part.block_address + i + DEVICE_OFFSET_SECT_SZ, &ptrBytes[i]);
    return *((uint32_t*)&ptrBytes[0]);
}

void fsDeviceFormat(struct Partition part, uint32_t begin, uint32_t end, uint32_t sectorSize) {
    // Zero the array of sectors
    for (unsigned int i=0; i < end; i++) 
        fs_write_byte(part.block_address + i, ' ');
    
    part.block_size = end - begin;
    part.sector_size = sectorSize;
    part.sector_count = part.block_size / part.sector_size;
    
    // Mark sectors as empty
    for (uint32_t i=0; i < part.sector_count; i++) 
        fs_write_byte(part.block_address + (i * part.sector_size), SECTOR_FREE);
    
    // Initiate device header and associated information
    for (uint32_t i=0; i < sizeof(DeviceHeaderString); i++) 
        fs_write_byte(part.block_address + i, DeviceHeaderString[i]);
    
    // Set device capacity
    uint8_t sizeBytes[4];
    *((uint32_t*)&sizeBytes[0]) = part.block_size;
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(part.block_address + i + DEVICE_OFFSET_CAPACITY, sizeBytes[i]);
    
    // Set the device type
    uint8_t deviceType = 'T';
    fs_write_byte(part.block_address + DEVICE_OFFSET_TYPE, deviceType);
    
    // Set the root directory pointer
    DirectoryHandle handle = fsDirectoryCreate(part, (uint8_t*)"root");
    
    uint8_t ptrBytes[4];
    *((uint32_t*)&ptrBytes[0]) = handle;
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(part.block_address + i + DEVICE_OFFSET_ROOT, ptrBytes[i]);
    
    // Set the sector size
    *((uint32_t*)&sizeBytes[0]) = part.sector_size;
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(part.block_address + i + DEVICE_OFFSET_SECT_SZ, sizeBytes[i]);
    
    return;
}

