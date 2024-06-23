#include <kernel/kernel.h>

void(*__fs_read_byte)(struct Bus*, uint32_t, uint8_t*);
void(*__fs_write_byte)(struct Bus*, uint32_t, uint8_t);

struct Bus fs_bus;

uint32_t fs_device_address;

uint8_t fs_device_root;


void fsSetDeviceLetter(uint8_t letter) {
    
    lowercase(&letter);
    
    fs_device_root = letter;
    
    if (letter == 'x') {
        
        fsSetDeviceTypeMEM();
        fs_device_address = 0x00000;
        
    } else {
        
        fsSetDeviceTypeIO();
        fs_device_address = PERIPHERAL_ADDRESS_BEGIN + ((letter - 'a') * 0x10000);
        
    }
    
    return;
}

void fsSetDeviceRoot(uint8_t deviceLetter) {
    
    fs_device_root = deviceLetter;
    
    return;
}

uint8_t fsGetDeviceRoot(void) {
    
    uint8_t tempCaseConv = fs_device_root;
    
    uppercase(&tempCaseConv);
    
    return tempCaseConv;
}

uint32_t fsGetDevice(void) {
    
    return fs_device_address;
}

void fsSetDevice(uint32_t device_address) {
    
    fs_device_address = device_address;
    
    return;
}

void fsInit(void) {
    
    fsSetDeviceTypeMEM();
    fs_device_address = 0;
    
    fsSetDeviceRoot('X');
    
    fs_bus.read_waitstate  = 5;
    fs_bus.write_waitstate = 5;
    
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


uint8_t fsCheckDeviceReady(void) {
    
    // Check header byte
    uint8_t headerByte[3];
    
    fs_read_byte(0, &headerByte[0]);
    fs_read_byte(1, &headerByte[1]);
    fs_read_byte(2, &headerByte[2]);
    
    if (headerByte[0] != 0x13) return 0;
    if (headerByte[1] != 'f') return 0;
    if (headerByte[2] != 's') return 0;
    
    return 1;
}


uint32_t fsGetDeviceCapacity(void) {
    
    union Pointer sizePointer;
    
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(DEVICE_CAPACITY_OFFSET + i, &sizePointer.byte_t[i]);
    
    return sizePointer.address;
}
