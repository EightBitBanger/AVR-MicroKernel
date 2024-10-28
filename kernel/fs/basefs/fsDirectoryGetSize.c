#include <kernel/kernel.h>

uint32_t fsDirectoryGetSize(uint32_t directoryAddress) {
    
    union Pointer directorySizePtr;
    
    fs_read_byte(directoryAddress + FILE_OFFSET_SIZE,     &directorySizePtr.byte_t[0]);
    fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + 1, &directorySizePtr.byte_t[1]);
    fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + 2, &directorySizePtr.byte_t[2]);
    fs_read_byte(directoryAddress + FILE_OFFSET_SIZE + 3, &directorySizePtr.byte_t[3]);
    
    return directorySizePtr.address;
}
