#ifndef _FILE_SYSTEM_BASE__
#define _FILE_SYSTEM_BASE__

#include <stdint.h>
#include <kernel/kalloc.h>

// Device types
#define FORMAT_DEVICE_GENERIC     'G'
#define FORMAT_DEVICE_EEPROM      'E'
#define FORMAT_DEVICE_FLASH       'F'
#define FORMAT_DEVICE_SSD         'S'

// Capacities

#define FORMAT_CAPACITY_32K       32768
#define FORMAT_CAPACITY_16K       16384
#define FORMAT_CAPACITY_8K        8192

// Sector format layout

#define FORMAT_SECTOR_SIZE        32

#define FORMAT_SECTOR_HEADER      'U'
#define FORMAT_SECTOR_DATA        'D'
#define FORMAT_SECTOR_FOOTER      'A'
#define FORMAT_SECTOR_EMPTY       '?'

// Device header layout

#define DEVICE_IDENTIFIER         {0x13, 'f', 's', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}

#define DEVICE_OFFSET_CAPACITY    12 // uint32     Max capacity of the device in bytes
#define DEVICE_OFFSET_TYPE        16 // uint8      Device type specifier
#define DEVICE_OFFSET_ROOT        17 // uint32     Pointer to the root directory

// File header layout

#define FILE_NAME_LENGTH          10

#define FILE_OFFSET_NAME          1  // 10 bytes   File name offset
#define FILE_OFFSET_SIZE          11 // uint32     File size offset
#define FILE_OFFSET_ATTRIBUTES    15 // uint32     File attribute offest
#define FILE_OFFSET_REF_COUNT     19 // uint32     Number of file references in this directory (if its a directory)
#define FILE_OFFSET_FLAG          23 // uint8      Flags (bitmask)
#define FILE_OFFSET_PARENT        24 // uint32     Pointer to parent block address
#define FILE_OFFSET_NEXT          28 // uint32     Pointer to next block address

// Directory layout
#define FS_DIRECTORY_REF_MAX           (FORMAT_SECTOR_SIZE / 4)
#define FS_DIRECTORY_LISTING_MAX       32768

// Working directory
#define WORKNG_DIRECTORY_STACK_SIZE    16

// User definable flags
#define FS_FLAG_A         0
#define FS_FLAG_B         1
#define FS_FLAG_C         2
#define FS_FLAG_D         3
#define FS_FLAG_E         4
#define FS_FLAG_F         5
#define FS_FLAG_G         6
#define FS_FLAG_H         7

#define ATTRIBUTE_OFFSET_FILETYPE   15
#define ATTRIBUTE_OFFSET_READ       16
#define ATTRIBUTE_OFFSET_WRITE      17
#define ATTRIBUTE_OFFSET_SPECIAL    18

struct FSAttribute {
    
    /// Allow the file to be executed.
    uint8_t executable;
    
    /// Allow read access.
    uint8_t readable;
    
    /// Allow write access.
    uint8_t writeable;
    
    /// Type attribute.
    /// 'd'  Directory
    uint8_t type;
    
};


void fsInit(void);


// Low level IO routing

void fs_write_byte(uint32_t address, uint8_t byte);
void fs_read_byte(uint32_t address, uint8_t* buffer);

void fs_set_type_IO(void);
void fs_set_type_EEPROM(void);
void fs_set_type_MEM(void);
void fs_set_type_MEM_nocache(void);

// Device context

/// Set device context by address offset.
void fsDeviceSetContext(uint32_t device_address);
uint32_t fsDeviceGetContext(void);

/// Set device context by letter.
void fsDeviceSetRootLetter(uint8_t deviceLetter);
uint8_t fsDeviceGetRootLetter(void);

/// Get information about the device.
uint8_t fsDeviceCheckReady(void);

/// Get the size of the device in bytes.
uint32_t fsDeviceGetCapacity(void);
/// Get the root directory of the device.
uint32_t fsDeviceGetRootContextDirectory(void);
/// Set the root directory of the device.
void fsDeviceSetRootContextDirectory(uint32_t directoryAddress);

// Allocation

/// Allocate a block of memory of the given size.
uint32_t fsAllocate(uint32_t size);
/// Free a block of memory.
uint8_t fsFree(uint32_t address);

// Format

/// Zero the bytes on the device and initiates the 
/// device sectors for allocation.
uint8_t fsFormat(uint32_t addressBegin, uint32_t addressEnd);
/// Initiate the device sectors without zeroing 
/// all the bytes on the device.
uint8_t fsFormatQuick(uint32_t addressBegin, uint32_t addressEnd);

/// Initiate the first sector on the device.
/// The first sector is reserved for the allocation table.
uint32_t fsDeviceConstructAllocationTable(uint32_t addressBegin, uint32_t addressEnd);

// Files

/// Allocate some space for the file size and return the location of the file.
uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize);
/// Free the file by its filename.
uint32_t fsFileDelete(uint8_t* name, uint8_t nameLength);
/// Check if a file exists in the working directory.
uint32_t fsFileExists(uint8_t* name, uint8_t nameLength);

/// Copy a file from the working directory to the destination
/// location at 'deviceLetter' into the subdirectory 'directoryAddress'.
uint32_t fsFileCopy(uint8_t* sourceName, uint8_t sourceLength, 
                    uint8_t* destName, uint8_t destLength, 
                    uint8_t deviceLetter, uint32_t directoryAddress);

/// Get the file name from its address.
void fsFileGetName(uint32_t fileAddress, uint8_t* name);
/// Set the file name at its address.
void fsFileSetName(uint32_t fileAddress, uint8_t* name, uint8_t length);
/// Get the size of the file in bytes.
uint32_t fsFileGetSize(uint32_t fileAddress);

// Attributes

/// Set the files attributes.
uint8_t fsFileSetAttributes(uint32_t address, struct FSAttribute* attributes);
/// Get the files attributes.
uint8_t fsFileGetAttributes(uint32_t address, struct FSAttribute* attributes);

// File flags

/// Get a bit from a files flag bitmask.
uint8_t fsFileGetFlag(uint32_t fileAddress, uint8_t index);
/// Set a bit at the index in a files flag bitmask.
/// State should be either 0 or not 0.
void fsFileSetFlag(uint32_t fileAddress, uint8_t index, uint8_t state);

// Search

/// Find the first file in a directory search.
uint32_t fsFileFindFirst(void);

/// Find the next file in the sequence.
uint32_t fsFileFindNext(void);
/// Get a file from an index location in the working directory.
uint32_t fsFileFind(uint32_t index);

// File buffer IO

/// Open a file for reading and writing. The file index is returned.
int32_t fsFileOpen(uint32_t fileAddress);
/// Close the file.
uint8_t fsFileClose(int32_t index);

/// Read bytes from a file into a buffer.
uint8_t fsFileRead(int32_t index, uint8_t* buffer, uint32_t size);
/// Write bytes from a buffer into a file.
uint8_t fsFileWrite(int32_t index, uint8_t* buffer, uint32_t size);

// Directories

/// Create a directory.
uint32_t fsDirectoryCreate(uint8_t* name, uint8_t nameLength);
/// Remove a directory without removing its contents.
uint32_t fsDirectoryDelete(uint8_t* name, uint8_t nameLength);
/// Remove a directories contents.
uint8_t fsDirectoryDeleteContents(uint32_t directoryAddress);
/// Check if a directory exists.
uint32_t fsDirectoryExists(uint8_t* name, uint8_t nameLength);

/// Get a file reference from a directory at a given index.
uint32_t fsDirectoryGetFile(uint32_t directoryAddress, uint32_t index);
/// Set a file reference in a directory at a given index.
uint8_t fsDirectorySetFile(uint32_t directoryAddress, uint32_t index, uint32_t fileAddress);

/// Add a file reference to a directory.
uint8_t fsDirectoryAddFile(uint32_t directoryAddress, uint32_t fileAddress);
/// Remove a file reference from a directory.
uint8_t fsDirectoryRemoveFile(uint32_t directoryAddress, uint32_t fileAddress);

/// Get the size of a directory.
uint32_t fsDirectoryGetSize(uint32_t directoryAddress);
/// Get the total size of a directory.
uint32_t fsDirectoryGetCapacity(uint32_t directoryAddress);
/// Get the number of contents in a directory.
uint32_t fsDirectoryGetNumberOfFiles(uint32_t directoryAddress);
/// Set the number of contents in a directory.
void fsDirectorySetNumberOfFiles(uint32_t directoryAddress, uint32_t numberOfFiles);

/// Get the parent directory in the chain.
uint32_t fsDirectoryGetParent(uint32_t directoryAddress);
/// Get the next directory in the chain.
/// Zero indicates this is the last directory block.
uint32_t fsDirectoryGetNext(uint32_t directoryAddress);
/// Set the parent pointer in the directory chain.
void fsDirectorySetParent(uint32_t directoryAddress, uint32_t parentAddress);
/// Set the next pointer in the directory chain.
void fsDirectorySetNext(uint32_t directoryAddress, uint32_t nextAddress);


// Working directory

/// Change the working directory to the given directory name.
uint8_t fsWorkingDirectoryChange(uint8_t* directoryName, uint8_t nameLength);

/// Get the parent directory from the working directory.
uint32_t fsWorkingDirectoryGetParent(void);
/// Set the working directory to the parent directory of 
/// the current working directory.
uint8_t fsWorkingDirectorySetToParent(void);

/// Set the address to the current working directory.
void fsWorkingDirectorySetAddress(uint32_t address);
/// Get the address to the current working directory.
uint32_t fsWorkingDirectoryGetAddress(void);

/// Get the current number of directories in the full path.
uint8_t fsWorkingDirectoryGetStack(void);
/// Set the current number of directories in the full path.
void fsWorkingDirectorySetStack(uint8_t amount);

/// Set the name and length in bytes of a directory.
uint8_t fsWorkingDirectorySetName(uint32_t directoryAddress);
/// Get the name of the working directory.
uint8_t fsWorkingDirectoryGetName(uint8_t* directoryName);

/// Get the number of file/directory references in the working directory.
uint32_t fsWorkingDirectoryGetFileCount(void);
/// Return a file/directory reference from the working directory.
uint32_t fsWorkingDirectoryFind(uint8_t index);

/// Clear the working directory path and drop to the root directory.
void fsWorkingDirectoryClear(void);


#endif
