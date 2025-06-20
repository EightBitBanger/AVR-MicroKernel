<<<<<<< HEAD
#include <kernel/fs/fs.h>
uint8_t fsFree(uint32_t address) {
    
    // Get file size
    union Pointer sizePtr;
    for (uint8_t i = 0; i < 4; i++)
        fs_read_byte(address + FILE_OFFSET_SIZE + i, &sizePtr.byte_t[i]);
    
    // Calculate number of sectors to allocate
    uint32_t totalSectors = (sizePtr.address + (fs_sector_size-2)) / (fs_sector_size-1);
    
    // Mark sectors as free
    for (uint32_t s = 0; s < totalSectors; s++) {
        uint32_t sectorAddr = address + (s * fs_sector_size);
        fs_write_byte(sectorAddr + s, FORMAT_SECTOR_EMPTY);
    }
    
    return 0;
}
=======
#include <kernel/fs/fs.h>
uint8_t fsFree(uint32_t address) {
    
    // Get file size
    union Pointer sizePtr;
    for (uint8_t i = 0; i < 4; i++)
        fs_read_byte(address + FILE_OFFSET_SIZE + i, &sizePtr.byte_t[i]);
    
    // Calculate number of sectors to allocate
    uint32_t totalSectors = (sizePtr.address + (fs_sector_size-2)) / (fs_sector_size-1);
    
    // Mark sectors as free
    for (uint32_t s = 0; s < totalSectors; s++) {
        uint32_t sectorAddr = address + (s * fs_sector_size);
        fs_write_byte(sectorAddr + s, FORMAT_SECTOR_EMPTY);
    }
    
    return 0;
}
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
