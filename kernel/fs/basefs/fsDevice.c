#include <kernel/fs/fs.h>

void(*__fs_read_byte)(struct Bus*, uint32_t, uint8_t*);
void(*__fs_write_byte)(struct Bus*, uint32_t, uint8_t);

struct Bus fs_bus;

struct DeviceDescriptor {
    
    
    
};



uint32_t fs_device_address;    // Base address of the device
uint8_t  fs_device_root;       // Character that represents the device
uint32_t fs_sector_size = 32;  // Sector size on the device

extern uint32_t fileBeginAddress[16]; // Open file addresses
extern uint32_t fileSize[16];


void fsDeviceSetRootLetter(uint8_t deviceLetter) {
    lowercase(&deviceLetter);
    
    fs_device_root = deviceLetter;
    fs_device_address = 0xffffffff;
    
    // Memory device
    if (deviceLetter == 'x') {
        fs_set_type_MEM();
        fs_device_address = 0x00000000;
    }
    
    // IO device
    if (fs_device_address == 0xffffffff) {
        fs_set_type_IO();
        fs_device_address = PERIPHERAL_ADDRESS_BEGIN + ((deviceLetter - 'a') * 0x10000);
    }
    
    return;
}

uint8_t fsDeviceGetRootLetter(void) {
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
    fs_bus.read_waitstate  = 2;
    fs_bus.write_waitstate = 2;
    
    fsDeviceSetRootLetter('x');
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
    
    __fs_read_byte  = bus_read_byte;
    __fs_write_byte = bus_write_byte_eeprom;
    
    return;
}

void fs_set_type_MEM(void) {
    __fs_read_byte  = bus_read_memory;
    __fs_write_byte = bus_write_memory;
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
        if (headerByte[i] != deviceID[i]) 
            return 0;
    
    // Get the device sector size
    fs_sector_size = fsDeviceGetSectorSize();
    
    // Check device root directory
    uint32_t rootDirectory = fsDeviceGetRootContextDirectory();
    
    if (rootDirectory == 0) 
        return 0;
    
    // Check device attributes
    struct FSAttribute attr;
    fsFileGetAttributes(rootDirectory, &attr);
    
    if ((attr.executable != ' ') | (attr.readable != 'r') | (attr.writeable != 'w') | (attr.type != 'd')) 
        return 0;
    
    uint32_t workingDirectory = fsWorkingDirectoryGetAddress();
    
    // Check working directory is valid
    if (workingDirectory != 0) {
        fsFileGetAttributes(workingDirectory, &attr);
        
        if ((attr.executable != ' ') | (attr.readable != 'r') | (attr.writeable != 'w') | (attr.type != 'd')) 
            return 0;
        
    }
    
    // Update root working directory
    if (fsWorkingDirectoryGetStack() == 0) 
        fsWorkingDirectorySetAddress(rootDirectory);
    return 1;
}


uint32_t fsDeviceGetCapacity(void) {
    union Pointer sizePointer;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(DEVICE_OFFSET_CAPACITY + i, &sizePointer.byte_t[i]);
    return sizePointer.address;
}


uint32_t fsDeviceGetSectorSize(void) {
    union Pointer deviceSectorSize;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(DEVICE_OFFSET_SECT_SZ + i, &deviceSectorSize.byte_t[i]);
    
    if (deviceSectorSize.address < 32) 
        deviceSectorSize.address = 32;
    return deviceSectorSize.address;
}


void fsDeviceSetRootContextDirectory(uint32_t directoryAddress) {
    union Pointer rootDirPtr;
    rootDirPtr.address = directoryAddress;
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(DEVICE_OFFSET_ROOT + i, rootDirPtr.byte_t[i]);
    for (uint8_t i=0; i < 4; i++) 
        fs_write_byte(directoryAddress + FILE_OFFSET_PARENT + i, 0);
    return;
}


uint32_t fsDeviceGetRootContextDirectory(void) {
    union Pointer rootDirPtr;
    for (uint8_t i=0; i < 4; i++) 
        fs_read_byte(DEVICE_OFFSET_ROOT + i, &rootDirPtr.byte_t[i]);
    return rootDirPtr.address;
}

