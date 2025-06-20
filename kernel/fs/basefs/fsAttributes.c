<<<<<<< HEAD
#include <kernel/fs/fs.h>

uint8_t fsFileSetAttributes(uint32_t address, struct FSAttribute* attributes) {
    
    uint8_t* attribBegin = (uint8_t*)attributes;
    
    for (uint8_t i=0; i < sizeof(struct FSAttribute); i++) 
        fs_write_byte(address + FILE_OFFSET_ATTRIBUTES + i, attribBegin[i]);
    
    return 1;
}


uint8_t fsFileGetAttributes(uint32_t address, struct FSAttribute* attributes) {
    
    uint8_t* attribBegin = (uint8_t*)attributes;
    
    for (uint8_t i=0; i < sizeof(struct FSAttribute); i++) 
        fs_read_byte(address + FILE_OFFSET_ATTRIBUTES + i, &attribBegin[i]);
    
    return 1;
}

=======
#include <kernel/fs/fs.h>

uint8_t fsFileSetAttributes(uint32_t address, struct FSAttribute* attributes) {
    
    uint8_t* attribBegin = (uint8_t*)attributes;
    
    for (uint8_t i=0; i < sizeof(struct FSAttribute); i++) 
        fs_write_byte(address + FILE_OFFSET_ATTRIBUTES + i, attribBegin[i]);
    
    return 1;
}


uint8_t fsFileGetAttributes(uint32_t address, struct FSAttribute* attributes) {
    
    uint8_t* attribBegin = (uint8_t*)attributes;
    
    for (uint8_t i=0; i < sizeof(struct FSAttribute); i++) 
        fs_read_byte(address + FILE_OFFSET_ATTRIBUTES + i, &attribBegin[i]);
    
    return 1;
}

>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
