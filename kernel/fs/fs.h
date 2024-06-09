#ifndef _FILE_SYSTEM_BASE__
#define _FILE_SYSTEM_BASE__

//
// File system selection

#define KERNEL_FILESYSTEM_BASE_FS

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
#define OFFSET_DIRECTORY_SIZE     19
#define OFFSET_DIRECTORY_FLAG     23

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


void fsInit(void);

void fs_write_byte(uint32_t address, uint8_t byte);
void fs_read_byte(uint32_t address, uint8_t* buffer);

// Current working directory

void fsClearWorkingDirectory(void);
void fsSetWorkingDirectory(uint8_t* directoryName, uint8_t nameLength);
uint8_t fsGetWorkingDirectory(uint8_t* directoryName);
uint8_t fsGetWorkingDirectoryLength(void);

uint32_t fsGetWorkingDirectoryAddress(void);
void fsSetWorkingDirectoryAddress(uint32_t address);

uint8_t fsCheckWorkingDirectory(void);

// Directory stack

void fsSetDirectoryStack(uint8_t amount);
uint8_t fsGetDirectoryStack(void);

// Device context

uint8_t fsGetRootDirectory(void);
void fsSetRootDirectory(uint8_t device);

void fsSetDeviceTypeIO(void);
void fsSetDeviceTypeMEM(void);

void fsSetDevice(uint32_t address);
void fsSetDeviceLetter(uint8_t letter);
uint32_t fsGetDevice(void);

uint32_t fsGetDeviceCapacity(void);
uint8_t fsCheckDeviceReady(void);

// Sectors

uint8_t fsSectorGetByte(uint32_t address);
void fsSectorSetByte(uint32_t address, uint8_t byte);


// File

uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize, uint8_t subType);
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
uint8_t fsFileReadText(uint8_t index, uint8_t* buffer, uint32_t length);
uint8_t fsFileReadBin(uint8_t index, uint8_t* buffer, uint32_t length);

// Directory

uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength);
uint32_t fsDirectoryDelete(uint8_t* name, uint8_t nameLength);

uint32_t fsDirectoryGetNumberOfFiles(uint8_t* name, uint8_t nameLength);
uint8_t fsDirectorySetNumberOfFiles(uint8_t* name, uint8_t nameLength, uint32_t numberOfFiles);

uint8_t fsDirectorySetFlag(uint8_t* name, uint8_t nameLength, uint8_t flag);
uint8_t fsDirectoryGetFlag(uint8_t* name, uint8_t nameLength);

uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength);
uint32_t fsDirectoryFileExists(uint8_t* name, uint8_t nameLength);

#endif
