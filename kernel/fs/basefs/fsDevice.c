#include <kernel/kernel.h>

#define FS_DEVICE_TYPE_IO    0
#define FS_DEVICE_TYPE_MEM   1

void(*__fs_read_byte)(struct Bus*, uint32_t, uint8_t*);
void(*__fs_write_byte)(struct Bus*, uint32_t, uint8_t);


uint32_t fs_device_address = 0;

uint8_t  fs_device_root = '/';

uint8_t  fs_working_directory[FILE_NAME_LENGTH];
uint8_t  fs_working_directory_length = 0;
uint32_t fs_working_directory_address;

uint8_t directoryStackPtr = 0;

struct Bus fs_bus;

void fsInit(void) {
    
    fs_bus.read_waitstate  = 5;
    fs_bus.write_waitstate = 5;
    
    fsSetDeviceTypeMEM();
    fsSetDeviceLetter('/');
    
    //
    // Format RAMDISK sectors
    //
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    uint32_t deviceCapacityBytes = 8192;
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
        fs_working_directory[i] = ' ';
    
    fs_working_directory_length = 0;
    
    for (sector=1; sector < 8192; sector++) {
        
        if (sectorCounter < (SECTOR_SIZE - 1)) {
            fs_write_byte( &fs_bus, sector, ' ');
            sectorCounter++;
        } else {
            fs_write_byte( &fs_bus, sector, 0x00);
            sectorCounter = 0;
        }
        
        continue;
    }
    
    //
    // Initiate first sector
    //
    
    fs_write_byte( &fs_bus, fs_device_address    , 0x13 );
    fs_write_byte( &fs_bus, fs_device_address + 1, 'f' );
    fs_write_byte( &fs_bus, fs_device_address + 2, 's' );
    
    // Device total capacity
    union Pointer deviceSize;
    
    deviceSize.address = deviceCapacityBytes;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte( &fs_bus, fs_device_address + DEVICE_CAPACITY_OFFSET + i, deviceSize.byte_t[i] );
    
    fsSetDeviceTypeIO();
    fsSetDeviceLetter('A');
    
    return;
}

void fs_read_byte(struct Bus* bus, uint32_t address, uint8_t* buffer) {
    
    __fs_read_byte(&fs_bus, address, buffer);
    
    return;
}

void fs_write_byte(struct Bus* bus, uint32_t address, uint8_t byte) {
    
    __fs_write_byte(&fs_bus, address, byte);
    
    return;
}

void fsSetDirectoryStack(uint8_t amount) {
    
    directoryStackPtr = amount;
    
    return;
}

uint8_t fsGetDirectoryStack(void) {
    
    return directoryStackPtr;
}

void fsSetRootDirectory(uint8_t device) {
    
    fs_device_root = device;
    
    return;
}

uint8_t fsGetRootDirectory(void) {
    
    return fs_device_root;
}


void fsClearWorkingDirectory(void) {
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
        fs_working_directory[i] = ' ';
    
    fs_working_directory_length = 0;
    
    fs_working_directory_address = 0;
    
    return;
}

void fsSetWorkingDirectory(uint8_t* directoryName, uint8_t nameLength) {
    
    uint32_t fileAddress = fsFileExists(directoryName, nameLength);
    
    if (fileAddress == 0) 
        return;
    
    fs_working_directory_address = fileAddress;
    
    fsClearWorkingDirectory();
    
    if (nameLength > FILE_NAME_LENGTH) 
        nameLength = FILE_NAME_LENGTH;
    
    for (uint8_t i=0; i < nameLength + 1; i++) 
        fs_working_directory[i] = directoryName[i];
    
    fs_working_directory_length = nameLength + 1;
    return;
}

uint8_t fsGetWorkingDirectory(uint8_t* directoryName) {
    
    for (uint8_t i=0; i < fs_working_directory_length; i++) 
        directoryName[i] = fs_working_directory[i];
    
    return fs_working_directory_length;
}

uint8_t fsGetWorkingDirectoryLength(void) {
    
    return fs_working_directory_length;
}

uint32_t fsGetWorkingDirectoryAddress(void) {
    
    return fs_working_directory_address;
}

void fsSetWorkingDirectoryAddress(uint32_t address) {
    
    fs_working_directory_address = address;
    
    return;
}

void fsSetDeviceTypeIO(void) {
    
    __fs_read_byte  = bus_read_io;
    __fs_write_byte = bus_write_io_eeprom;
    
    return;
}

void fsSetDeviceTypeMEM(void) {
    
    __fs_read_byte  = bus_read_memory;
    __fs_write_byte = bus_write_memory;
    
    return;
}

void fsSetDevice(uint32_t address) {
    
    fs_device_address = address;
    
    return;
}

uint32_t fsGetDevice(void) {
    
    return fs_device_address;
}

void fsSetDeviceLetter(uint8_t letter) {
    
    if (letter == '/') {
        
        fsSetDeviceTypeMEM();
        fs_device_address = 0x00000;
        
    } else {
        
        lowercase(&letter);
        
        fsSetDeviceTypeIO();
        fs_device_address = PERIPHERAL_ADDRESS_BEGIN + ((letter - 'A') * 0x10000);
        
    }
    
    return;
}



uint8_t fsCheckDeviceReady(void) {
    
    uint32_t currentDevice = fsGetDevice();
    
    // Check header byte
    uint8_t headerByte[3];
    
    fs_read_byte(&fs_bus, currentDevice + 0, &headerByte[0]);
    fs_read_byte(&fs_bus, currentDevice + 1, &headerByte[1]);
    fs_read_byte(&fs_bus, currentDevice + 2, &headerByte[2]);
    
    if (headerByte[0] != 0x13) return 0;
    if (headerByte[1] != 'f') return 0;
    if (headerByte[2] != 's') return 0;
    
    return 1;
}



uint8_t fsGetSectorByte(uint32_t address) {
    uint8_t headerByte = 0;
    
    fs_read_byte(&fs_bus, fs_device_address + address, &headerByte);
    
    return headerByte;
}



uint32_t fsGetDeviceCapacity(void) {
    
    uint32_t currentDevice = fsGetDevice();
    
    // Check header byte
    
    // Get device capacity
    union Pointer sizePointer;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(&fs_bus, currentDevice + DEVICE_CAPACITY_OFFSET + i, &sizePointer.byte_t[i]);
    
    return sizePointer.address;
}
