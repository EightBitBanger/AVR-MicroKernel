#ifndef _FILE_SYSTEM_BASE__
#define _FILE_SYSTEM_BASE__

// Capacities

#define CAPACITY_32K      1024
#define CAPACITY_16K      64
#define CAPACITY_8K       32

// Format

#define SECTOR_SIZE               32

#define DEVICE_CAPACITY_OFFSET    12

// File layout

#define FILE_NAME_LENGTH          10

#define OFFSET_FILE_SIZE          11
#define OFFSET_FILE_ATTRIBUTES    15

#define FILE_ATTRIBUTE_FILETYPE   15
#define FILE_ATTRIBUTE_READ       16
#define FILE_ATTRIBUTE_WRITE      17
#define FILE_ATTRIBUTE_SPECIAL    18


void fsSetCurrentDevice(uint8_t device_index);

uint8_t fsGetDeviceHeaderByte(uint32_t address_offset);

uint32_t fsGetDeviceCapacity(void);

uint32_t fsGetCurrentDevice(void);


uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize);


void fsListDirectory(void);

uint8_t fsRepairDevice(void);

#endif
