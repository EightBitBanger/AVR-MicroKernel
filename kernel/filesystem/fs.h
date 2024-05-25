#ifndef _FILE_SYSTEM_BASE__
#define _FILE_SYSTEM_BASE__

// Capacities

#define CAPACITY_32K      256
#define CAPACITY_16K      128
#define CAPACITY_8K       64

// Sector format

#define SECTOR_SIZE               32

#define DEVICE_CAPACITY_OFFSET    12

// File layout

#define FILE_NAME_LENGTH          10

#define OFFSET_FILE_NAME          1
#define OFFSET_FILE_SIZE          11
#define OFFSET_FILE_ATTRIBUTES    15

#define FILE_ATTRIBUTE_FILETYPE   15
#define FILE_ATTRIBUTE_READ       16
#define FILE_ATTRIBUTE_WRITE      17
#define FILE_ATTRIBUTE_SPECIAL    18

struct FSAttribute {
    
    uint8_t executable;
    uint8_t readable;
    uint8_t writeable;
    
};


void fsInit(void);

void fs_write_byte(struct Bus* bus, uint32_t address, uint8_t byte);

void fs_read_byte(struct Bus* bus, uint32_t address, uint8_t* buffer);


// Device context

void fsSetDeviceTypeIO(void);

void fsSetDeviceTypeMEM(void);

void fsSetCurrentDevice(uint8_t device_index);

uint8_t fsGetDeviceHeaderByte(uint32_t address_offset);

uint32_t fsGetDeviceCapacity(void);

uint32_t fsGetCurrentDevice(void);

uint8_t fsCheckDeviceReady(void);

// File manipulation

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize);

uint8_t fsFileDelete(uint8_t* name, uint8_t nameLength);

uint32_t fsFileExists(uint8_t* name, uint8_t nameLength);

uint32_t fsFileFind(uint32_t index);

uint8_t fsFileRename(uint8_t* name, uint8_t nameLength, uint8_t* newName, uint8_t newNameLength);

uint32_t fsGetFileSize(uint8_t* name, uint8_t nameLength);

uint8_t fsGetFileAttributes(uint8_t* name, uint8_t nameLength, struct FSAttribute* attributes);

uint8_t fsSetFileAttributes(uint8_t* name, uint8_t nameLength, struct FSAttribute* attributes);

// File IO

uint8_t fsFileOpen(uint8_t* name, uint8_t nameLength);

uint8_t fsFileSeekGet(void);

uint8_t fsFileSeekSet(uint8_t position);

uint8_t fsFileClose(uint8_t index);

uint8_t fsFileWrite(uint8_t index, uint8_t* buffer, uint32_t length);

uint8_t fsFileRead(uint8_t index, uint8_t* buffer, uint32_t length);


// Special

void fsListDirectory(void);

#endif
