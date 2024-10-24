#ifndef _FILE_SYSTEM_ATTRIBUTE__
#define _FILE_SYSTEM_ATTRIBUTE__

#define FILE_ATTRIBUTE_FILETYPE   15
#define FILE_ATTRIBUTE_READ       16
#define FILE_ATTRIBUTE_WRITE      17
#define FILE_ATTRIBUTE_SPECIAL    18


struct FSAttribute {
    uint8_t executable;
    uint8_t readable;
    uint8_t writeable;
    uint8_t type;
};


uint8_t fsFileSetAttributes(uint32_t address, struct FSAttribute* attributes);

uint8_t fsFileGetAttributes(uint32_t address, struct FSAttribute* attributes);


#endif
