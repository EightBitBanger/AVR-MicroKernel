#include <kernel/kernel.h>

void(*__fs_read_byte)(struct Bus*, uint32_t, uint8_t*);
void(*__fs_write_byte)(struct Bus*, uint32_t, uint8_t);

struct Bus fs_bus;

uint32_t fs_device_address;

uint8_t fs_device_root;


void fsDeviceSetRoot(uint8_t deviceLetter) {
    
    lowercase(&deviceLetter);
    
    fs_device_root = deviceLetter;
    
    if (deviceLetter == 'x') {
        
        fs_set_type_MEM();
        fs_device_address = 0x00000;
        
    } else {
        
        fs_set_type_IO();
        fs_device_address = PERIPHERAL_ADDRESS_BEGIN + ((deviceLetter - 'a') * 0x10000);
        
    }
    
    return;
}

uint8_t fsDeviceGetRoot(void) {
    
    uint8_t tempCaseConv = fs_device_root;
    
    uppercase(&tempCaseConv);
    
    return tempCaseConv;
}

uint32_t fsDeviceGetContext(void) {
    
    return fs_device_address;
}

void fsDeviceSetContext(uint32_t device_address) {
    
    fs_device_address = device_address;
    
    return;
}

void fsInit(void) {
    
    fs_device_address = 0;
    
    fs_set_type_MEM();
    fsDeviceSetRoot('X');
    
    fs_bus.read_waitstate  = 4;
    fs_bus.write_waitstate = 4;
    
    return;
}

void fs_write_byte(uint32_t address, uint8_t byte) {
    
    __fs_write_byte(&fs_bus, fs_device_address + address, byte);
    
    return;
}

void fs_read_byte(uint32_t address, uint8_t* buffer) {
    
    __fs_read_byte(&fs_bus, fs_device_address + address, buffer);
    
    return;
}

void fs_set_type_IO(void) {
    
    __fs_read_byte  = bus_read_io;
    __fs_write_byte = bus_write_io_eeprom;
    
    return;
}

void fs_set_type_MEM(void) {
    
    __fs_read_byte  = bus_raw_read_memory;
    __fs_write_byte = bus_raw_write_memory;
    
    // TODO Cache glitch, bytes flipping randomly
    //__fs_read_byte  = bus_read_memory;
    //__fs_write_byte = bus_write_memory;
    
    return;
}

void fs_set_type_MEM_nocache(void) {
    
    __fs_read_byte  = bus_raw_read_memory;
    __fs_write_byte = bus_raw_write_memory;
    
    return;
}


uint8_t fsDeviceCheckReady(void) {
    
    uint8_t deviceID[] = DEVICE_IDENTIFIER;
    
    uint8_t headerByte[sizeof(deviceID)];
    
    for (uint8_t i=0; i < sizeof(deviceID); i++) 
        fs_read_byte(i, &headerByte[i]);
    
    for (uint8_t i=0; i < sizeof(deviceID); i++) 
        if (headerByte[i] != deviceID[i]) return 0;
    
    return 1;
}


uint32_t fsDeviceGetCapacity(void) {
    
    union Pointer sizePointer;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(DEVICE_CAPACITY_OFFSET + i, &sizePointer.byte_t[i]);
    
    return sizePointer.address;
}


void fsDeviceSetRootDirectory(uint32_t directoryAddress) {
    
    union Pointer rootDirPtr;
    rootDirPtr.address = directoryAddress;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(DEVICE_ROOT_OFFSET + i, rootDirPtr.byte_t[i]);
    
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(directoryAddress + FILE_OFFSET_PARENT + i, 0);
    
    return;
}


uint32_t fsDeviceGetRootDirectory(void) {
    
    union Pointer rootDirPtr;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(DEVICE_ROOT_OFFSET + i, &rootDirPtr.byte_t[i]);
    
    return rootDirPtr.address;
}

