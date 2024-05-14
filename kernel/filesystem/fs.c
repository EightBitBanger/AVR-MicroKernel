#include <avr/io.h>

#include <kernel/delay.h>

#include <kernel/kernel.h>

#include <kernel/filesystem/fs.h>
#include <kernel/filesystem/fs.h>

uint8_t deviceNotReady[]= "Device not ready";


uint32_t fs_device_address = 0;

void fsSetCurrentDevice(uint8_t device_index) {
    
    fs_device_address = PERIPHERAL_ADDRESS_BEGIN + (device_index * 0x10000);
    
    return;
}

uint32_t fsGetCurrentDevice(void) {
    return fs_device_address;
}

uint8_t fsCheckDeviceReady(void) {
    
    uint8_t deviceHeader[2];
    deviceHeader[0] = fsGetDeviceHeaderByte(1);
    deviceHeader[1] = fsGetDeviceHeaderByte(2);
    
    if ((deviceHeader[0] != 'f') | (deviceHeader[1] != 's')) 
        return 0;
    
    return 1;
}


uint8_t fsGetDeviceHeaderByte(uint32_t address_offset) {
    uint8_t headerByte = 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    bus_read_byte(&bus, fs_device_address + address_offset, &headerByte);
    
    return headerByte;
}

uint32_t fsGetDeviceCapacity(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    bus.write_waitstate = 5;
    
    uint8_t buffer[SECTOR_SIZE];
    
    // Get header sector
    for (uint8_t i=0; i < SECTOR_SIZE; i++) 
        bus_read_byte(&bus, fsGetCurrentDevice() + i, &buffer[i]);
    
    // Check header byte
    if (buffer[0] != 0x13) 
        return 0;
    
    // Check hardware name
    if ((buffer[1] != 'f') | (buffer[2] != 's')) 
        return 0;
    
    union Pointer sizePointer;
    
    // Get device capacity
    for (uint8_t i=0; i < 4; i++) 
        sizePointer.byte_t[i] = buffer[i + DEVICE_CAPACITY_OFFSET];
    
    return sizePointer.address;
}



//
// File open/close and read/write
//


uint32_t fileBeginAddress = 0;


uint8_t fsFileOpen(uint8_t* name, uint8_t nameLength) {
    
    fileBeginAddress = fsFileExists(name, nameLength);
    
    if (fileBeginAddress != 0)
        return 1;
    
    return 0;
}

uint8_t fsFileClose(uint8_t index) {
    
    if (fileBeginAddress != 0) {
        fileBeginAddress = 0;
        return 1;
    }
    
    return 0;
}

uint8_t fsFileWrite(uint8_t index, uint8_t* buffer, uint8_t length) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    bus.write_waitstate = 5;
    
    for (uint32_t i=0; i < length; i++) 
        bus_write_io_eeprom(&bus, fileBeginAddress + SECTOR_SIZE + i + 1, buffer[i]);
    
    return 1;
}

uint8_t fsFileRead(uint8_t index, uint8_t* buffer, uint8_t length) {
    
    if (fileBeginAddress == 0) 
        return 0;
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    bus.write_waitstate = 5;
    
    for (uint32_t i=0; i < length; i++) 
        bus_read_byte(&bus, fileBeginAddress + SECTOR_SIZE + i + 1, &buffer[i] );
    
    return ' ';
}





void fsListDirectory(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    uint8_t buffer[SECTOR_SIZE];
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    if (currentCapacity == 0) {
        print( &deviceNotReady[0], sizeof(deviceNotReady) );
        printLn();
        return;
    }
    
    // Default to minimum size if size unknown
    // Verify the capacity
    if ((currentCapacity != CAPACITY_8K) & 
        (currentCapacity != CAPACITY_16K) & 
        (currentCapacity != CAPACITY_32K)) {
        currentCapacity = CAPACITY_8K;
    }
    
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Get sector header byte
        uint8_t headerByte = 0;
        bus_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE), &headerByte);
        
        // Check active sector
        
        if (fsGetDeviceHeaderByte( sector * SECTOR_SIZE ) != 0x55) 
            continue;
        
        // Get header sector
        for (uint8_t i=0; i < SECTOR_SIZE; i++) 
            bus_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE) + i, &buffer[i]);
        
        // Get file size
        union Pointer fileSize;
        
        for (uint8_t i=0; i < 4; i++) 
            fileSize.byte_t[i] = buffer[ OFFSET_FILE_SIZE + i ];
        
        uint8_t integerString[8];
        uint8_t len = int_to_string(fileSize.address, integerString) + 1;
        
        if (len > 6) 
            len = 6;
        
        // Get file attributes
        uint8_t fileAttributes[4];
        
        for (uint8_t i=0; i < 4; i++) 
            fileAttributes[i] = buffer[ OFFSET_FILE_ATTRIBUTES + i ];
        
        // Check special attribute
        
        // Directory
        if (fileAttributes[0] == 'd') {
            
            print(&buffer[1], FILE_NAME_LENGTH + 1);
            
            printSpace(1);
            
            uint8_t directoryAttribute[] = "<dir>";
            print( &directoryAttribute[0], sizeof(directoryAttribute) );
            
        }
        
        // File
        if (fileAttributes[0] != 'd') {
            
            uint8_t integerOffset = 6 - len;
            
            if (integerOffset > 5) 
                integerOffset = 5;
            
            print( &fileAttributes[1], 4 );
            
            printSpace(1);
            
            print(&buffer[1], FILE_NAME_LENGTH + 1);
            
            printSpace(integerOffset);
            print( integerString, len);
            
            printSpace(1);
            
        }
        
        printLn();
        
        continue;
    }
    
    return;
}

