#include <kernel/kernel.h>

#define FS_DEVICE_TYPE_IO    0
#define FS_DEVICE_TYPE_MEM   1

void(*__fs_read_byte)(struct Bus*, uint32_t, uint8_t*);
void(*__fs_write_byte)(struct Bus*, uint32_t, uint8_t);


uint32_t fs_device_address;
struct Bus fs_bus;



uint8_t  fs_device_root;

uint8_t  fs_working_directory[FILE_NAME_LENGTH];
uint8_t  fs_working_directory_length;
uint32_t fs_working_directory_address;

uint8_t fs_directory_stack_ptr;


void fsInit(void) {
    
    fs_bus.read_waitstate  = 5;
    fs_bus.write_waitstate = 5;
    
    fs_device_root = 'X';
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
        fs_working_directory[i] = ' ';
    
    fs_working_directory_length = 0;
    fs_working_directory_address = 0;
    fs_device_address = 0;
    
    fs_directory_stack_ptr = 0;
    
    fsSetDeviceTypeMEM();
    fsSetDeviceLetter('X');
    
    //
    // Format RAMDISK sectors
    //
    
    uint32_t sector = 0;
    uint32_t sectorCounter = 0;
    
    uint32_t deviceCapacityBytes = 8192;
    
    for (uint8_t i=0; i < FILE_NAME_LENGTH; i++) 
        fs_working_directory[i] = ' ';
    
    fs_working_directory_length = 0;
    
    for (sector=1; sector < deviceCapacityBytes; sector++) {
        
        if (sectorCounter < (SECTOR_SIZE - 1)) {
            fsSectorSetByte(sector, ' ');
            sectorCounter++;
        } else {
            fsSectorSetByte(sector, 0x00);
            sectorCounter = 0;
        }
        
        continue;
    }
    
    //
    // Initiate first sector
    //
    
    fsSectorSetByte(fs_device_address    , 0x13 );
    fsSectorSetByte(fs_device_address + 1, 'f' );
    fsSectorSetByte(fs_device_address + 2, 's' );
    
    // Device total capacity
    union Pointer deviceSize;
    
    deviceSize.address = deviceCapacityBytes;
    
    for (uint8_t i=0; i < 4; i++) 
        fsSectorSetByte(fs_device_address + DEVICE_CAPACITY_OFFSET + i, deviceSize.byte_t[i]);
    
    return;
}

void fs_read_byte(uint32_t address, uint8_t* buffer) {
    
    __fs_read_byte(&fs_bus, address, buffer);
    
    return;
}

void fs_write_byte(uint32_t address, uint8_t byte) {
    
    __fs_write_byte(&fs_bus, address, byte);
    
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
    
    if (letter == 'X') {
        
        fsSetDeviceTypeMEM();
        fs_device_address = 0x00000;
        
    } else {
        
        lowercase(&letter);
        
        fsSetDeviceTypeIO();
        fs_device_address = PERIPHERAL_ADDRESS_BEGIN + ((letter - 'a') * 0x10000);
        
    }
    
    return;
}



uint8_t fsCheckDeviceReady(void) {
    
    uint32_t currentDevice = fsGetDevice();
    
    // Check header byte
    uint8_t headerByte[3];
    
    fs_read_byte(currentDevice + 0, &headerByte[0]);
    fs_read_byte(currentDevice + 1, &headerByte[1]);
    fs_read_byte(currentDevice + 2, &headerByte[2]);
    
    if (headerByte[0] != 0x13) return 0;
    if (headerByte[1] != 'f') return 0;
    if (headerByte[2] != 's') return 0;
    
    return 1;
}


uint32_t fsGetDeviceCapacity(void) {
    
    uint32_t currentDevice = fsGetDevice();
    
    union Pointer sizePointer;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(currentDevice + DEVICE_CAPACITY_OFFSET + i, &sizePointer.byte_t[i]);
    
    return sizePointer.address;
}
