#ifndef _FILE_SYSTEM_BASE__
#define _FILE_SYSTEM_BASE__

#include <kernel/fs/fsAttribute.h>

// Capacities

#define FORMAT_CAPACITY_32K       32768
#define FORMAT_CAPACITY_16K       16384
#define FORMAT_CAPACITY_8K        8192

// Sector format

#define FORMAT_SECTOR_SIZE        32

#define FORMAT_SECTOR_HEADER      0x55
#define FORMAT_SECTOR_DATA        0xff
#define FORMAT_SECTOR_FOOTER      0xaa
#define FORMAT_SECTOR_EMPTY       0x00

#define DEVICE_CAPACITY_OFFSET    12
#define DEVICE_ROOT_OFFSET        16

// Header layout

#define FILE_NAME_LENGTH          10

#define FILE_OFFSET_NAME          1  // 10 bytes   File name offset
#define FILE_OFFSET_SIZE          11 // uint32     File size offset
#define FILE_OFFSET_ATTRIBUTES    15 // 4 bytes    File attribute offest
#define FILE_OFFSET_REF_COUNT     19 // uint32     Number of file references in this directory (if its a directory)
#define FILE_OFFSET_FLAG          23 // uint8      Flags (bitmask)

// Working directory
#define WORKNG_DIRECTORY_STACK_SIZE    16

// Flags
#define FS_FLAG_A         0
#define FS_FLAG_B         1
#define FS_FLAG_C         2
#define FS_FLAG_D         3
#define FS_FLAG_E         4
#define FS_FLAG_F         5
#define FS_FLAG_G         6
#define FS_FLAG_H         7


void fsInit(void);


// Device context

void fs_write_byte(uint32_t address, uint8_t byte);
void fs_read_byte(uint32_t address, uint8_t* buffer);

void fs_set_type_IO(void);
void fs_set_type_MEM(void);
void fs_set_type_MEM_nocache(void);

uint32_t fsDeviceGetContext(void);
void fsDeviceSetContext(uint32_t device_address);

uint32_t fsDeviceGetCapacity(void);
uint8_t fsDeviceCheckReady(void);
void fsDeviceSetLetter(uint8_t letter);

void fsDeviceSetRoot(uint8_t deviceLetter);
uint8_t fsDeviceGetRoot(void);

uint32_t fsDeviceGetRootDirectory(void);
void fsDeviceSetRootDirectory(uint32_t directoryAddress);


// Allocation

uint32_t fsAllocate(uint32_t size);
uint8_t fsFree(uint32_t address);


// Format

uint8_t fsFormat(uint32_t addressBegin, uint32_t addressEnd);

uint8_t fsDeviceConstructAllocationTable(uint32_t addressBegin, uint32_t addressEnd);


// Files

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize);
uint8_t fsFileDelete(uint8_t* name, uint8_t nameLength);

uint32_t fsFileExists(uint8_t* name, uint8_t nameLength);

uint8_t fsFileRename(uint8_t* name, uint8_t nameLength, uint8_t* newName, uint8_t newNameLength);

uint8_t fsFileGetName(uint32_t fileAddress, uint8_t* name);
uint32_t fsFileGetSize(uint32_t fileAddress);


// File flags

uint8_t fsFileGetFlag(uint32_t fileAddress, uint8_t index);
void fsFileSetFlag(uint32_t fileAddress, uint8_t index, uint8_t state);


// Search

uint32_t fsFileFindFirst(void);
uint32_t fsFileFindNext(void);
uint32_t fsFileFind(uint32_t index);


// Directories

uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength);
uint8_t fsDirectoryDelete(uint8_t* name, uint8_t nameLength);

uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength);

uint32_t fsDirectoryGetFileAtIndex(uint32_t directoryAddress, uint32_t index);
uint32_t fsDirectoryGetSize(uint32_t directoryAddress);
uint32_t fsDirectoryGetNumberOfFiles(uint32_t directoryAddress);



// File IO
uint8_t fsFileOpen(uint32_t fileAddress);
uint8_t fsFileClose(void);

uint8_t fsFileRead(uint8_t* buffer, uint32_t size);
uint8_t fsFileReadText(uint8_t* buffer, uint32_t size);
uint8_t fsFileWrite(uint8_t* buffer, uint32_t size);



// Working directory

uint32_t fsWorkingDirectoryGetParent(void);
uint8_t  fsSetWorkingDirectoryToParent(void);

void     fsWorkingDirectorySetAddress(uint32_t address);
uint32_t fsWorkingDirectoryGetAddress(void);

uint8_t fsWorkingDirectoryGetStack(void);
void    fsWorkingDirectorySetStack(uint8_t amount);

uint8_t fsGetWorkingDirectory(uint8_t* directoryName);
uint8_t fsSetWorkingDirectory(uint32_t directoryAddress);
uint8_t fsChangeWorkingDirectory(uint8_t* directoryName, uint8_t nameLength);

uint32_t fsWorkingDirectoryGetFileCount(void);
uint32_t fsWorkingDirectoryFind(uint8_t index);

uint8_t fsWorkingDirectoryGetLength(void);
void fsWorkingDirectorySetLength(uint8_t length);

uint8_t fsWorkingDirectoryCheck(void);

void fsWorkingDirectoryClear(void);



#endif
