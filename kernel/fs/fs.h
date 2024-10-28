#ifndef _FILE_SYSTEM_BASE__
#define _FILE_SYSTEM_BASE__

#include <kernel/fs/fsAttribute.h>

//
// File system selection

#define KERNEL_FILESYSTEM_BASE_FS

// Capacities

#define FORMAT_CAPACITY_32K       32768
#define FORMAT_CAPACITY_16K       16384
#define FORMAT_CAPACITY_8K        8192

// Sector format

#define SECTOR_SIZE               32

#define SECTOR_HEADER             0x55
#define SECTOR_DATA               0xff
#define SECTOR_FOOTER             0xaa
#define SECTOR_EMPTY              0x00

#define DEVICE_CAPACITY_OFFSET    12
#define DEVICE_ROOT_DIR_OFFSET    16

// File layout

#define FILE_NAME_LENGTH          10

#define FILE_OFFSET_NAME          1  // 10 bytes   File name offset
#define FILE_OFFSET_SIZE          11 // uint32     File size offset
#define FILE_OFFSET_ATTRIBUTES    15 // 4 bytes    File attribute offest

#define DIRECTORY_OFFSET_SIZE     19 // uint32     Number of files in the directory
#define DIRECTORY_OFFSET_FLAG     23 // uint8      Owned by a directory

// Working directory
#define WORKNG_DIRECTORY_STACK_SIZE    16

// Flags
#define FS_FLAG_OWNED_BY_DIR        0



void fsInit(void);


// Device context

void fs_write_byte(uint32_t address, uint8_t byte);
void fs_read_byte(uint32_t address, uint8_t* buffer);

uint32_t fsGetDevice(void);
void fsSetDevice(uint32_t device_address);

void fsSetDeviceTypeIO(void);
void fsSetDeviceTypeMEM(void);
void fsSetDeviceTypeMEMDirect(void);

uint32_t fsGetDeviceCapacity(void);
uint8_t fsCheckDeviceReady(void);
void fsSetDeviceLetter(uint8_t letter);
void fsSetDeviceRoot(uint8_t deviceLetter);
uint8_t fsGetDeviceRoot(void);

// Allocation

uint32_t fsAllocate(uint32_t size);
uint8_t fsFree(uint32_t address);


// Format

uint8_t fsFormat(uint32_t addressBegin, uint32_t addressEnd);

uint8_t fsFotmatConstructAllocationTable(uint32_t addressBegin, uint32_t addressEnd);

uint32_t fsFormatGetRootDirectory(void);
void fsFormatSetRootDirectory(uint32_t directoryAddress);



// Files

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize);
uint8_t fsFileDelete(uint8_t* name, uint8_t nameLength);

uint32_t fsFileExists(uint8_t* name, uint8_t nameLength);

uint8_t fsFileRename(uint8_t* name, uint8_t nameLength, uint8_t* newName, uint8_t newNameLength);

uint8_t fsFileGetName(uint32_t fileAddress, uint8_t* name);
uint32_t fsFileGetSize(uint32_t fileAddress);

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

uint32_t fsDirectoryGetFile(uint32_t directoryAddress, uint32_t index);
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

uint8_t fsCheckWorkingDirectory(void);

void fsClearWorkingDirectory(void);



#endif
