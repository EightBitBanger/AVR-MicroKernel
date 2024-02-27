#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/fs/fs.h>
#include <kernel/fs/fs.h>

uint32_t fs_device_address = 0x40000;

void fsSetCurrentDevice(uint8_t device_index) {
    
    fs_device_address = 0x40000 + (device_index * 0x10000);
    
    return;
}

uint32_t fsGetCurrentDevice(void) {
    return fs_device_address;
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
    
    uint8_t buffer[SECTOR_SIZE];
    
    // Get header sector
    for (uint8_t i=0; i < SECTOR_SIZE; i++) 
        bus_read_byte(&bus, fsGetCurrentDevice() + i, &buffer[i]);
    
    // Check header byte
    if (buffer[0] != 0x13) {
        return 0;
    }
    
    // Check hardware name
    if ((buffer[1] != 'f') | (buffer[2] != 's')) {
        return 0;
    }
    
    union Pointer sizePointer;
    
    // Get device capacity
    for (uint8_t i=0; i < 4; i++) 
        sizePointer.byte_t[i] = buffer[i + DEVICE_CAPACITY_OFFSET];
    
    return sizePointer.address;
}


uint32_t fsFileCreate(uint8_t* name, uint8_t nameLength, uint32_t fileSize) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    uint32_t freeSectorCount = 0;
    uint32_t fileTargetAddress = 0;
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    if (currentCapacity == 0) {
        uint8_t deviceNotReady[]= "Device not ready";
        print( &deviceNotReady[0], sizeof(deviceNotReady) );
        printLn();
        return 0;
    }
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    // Default to minimum size if size unknown
    // Verify the capacity
    if ((currentCapacity != CAPACITY_8K) & 
        (currentCapacity != CAPACITY_16K) & 
        (currentCapacity != CAPACITY_32K)) {
        currentCapacity = CAPACITY_8K;
    }
    
    // Verify max capacity
    if (currentCapacity > CAPACITY_32K) {
        currentCapacity = CAPACITY_32K;
    }
    
    // Calculate sectors required to fit the file
    uint32_t requiredSectors=0;
	for (uint32_t i=0; i < fileSize; i += (SECTOR_SIZE - 1)) {
		requiredSectors++;
	}
	
	if (requiredSectors == 0) 
		requiredSectors = 1;
	
    for (uint32_t sector=0; sector < currentCapacity; sector++) {
        
        // Get sector header byte
        uint8_t headerByte = 0;
        bus_read_byte(&bus, currentDevice + (sector * SECTOR_SIZE), &headerByte);
        
        // Find an empty sector
        if (fsGetDeviceHeaderByte( sector * SECTOR_SIZE ) != 0x00) 
            continue;
        
        // Find next sectors for total file size
        for (uint32_t nextSector = sector; nextSector < currentCapacity; nextSector++) {
            
            // Get sector header byte
            uint8_t headerByte = 0;
            bus_read_byte(&bus, currentDevice + (nextSector * SECTOR_SIZE), &headerByte);
            
            if (fsGetDeviceHeaderByte( nextSector * SECTOR_SIZE ) == 0x00) {
                
                // Check target reached
                if (freeSectorCount == requiredSectors) {
                    
                    fileTargetAddress = currentDevice + (nextSector * SECTOR_SIZE);
                    
                    break;
                }
                
                freeSectorCount++;
                
                continue;
            }
            
            freeSectorCount = 0;
            
            break;
        }
        
		// File cannot fit into this empty space, continue seeking free space
		if (requiredSectors != freeSectorCount) 
			continue;
		
		// Mark the first sector as taken
        uint8_t fileStartbyte = 0x55; // File start byte is 0x55
		bus_write_byte_eeprom(fileTargetAddress, fileStartbyte);
		
		// Write file name
		for (uint8_t i=0; i < nameLength; i++) 
            bus_write_byte_eeprom( fileTargetAddress + i + 1, name[i] );
		
        // Set file size
        union Pointer sizePtr;
        sizePtr.address = fileSize;
        
        for (uint8_t i=0; i < 4; i++) 
            bus_write_byte_eeprom( fileTargetAddress + i + OFFSET_FILE_SIZE, sizePtr.byte_t[i] );
        
        // Write file attributes
        uint8_t attributes[4] = {' ', ' ', 'r', 'w'};
        
        for (uint8_t i=0; i < 4; i++) 
            bus_write_byte_eeprom( fileTargetAddress + i + OFFSET_FILE_ATTRIBUTES, attributes[i] );
        
        return fileTargetAddress;
    }
    
    return 0;
}


void fsListDirectory(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    uint8_t buffer[SECTOR_SIZE];
    
    uint32_t currentCapacity = fsGetDeviceCapacity();
    
    if (currentCapacity == 0) {
        
        uint8_t deviceNotReady[]= "Device not ready";
        
        print( &deviceNotReady[0], sizeof(deviceNotReady) );
        
        printLn();
        
        return;
    }
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    // Default to minimum size if size unknown
    // Verify the capacity
    if ((currentCapacity != CAPACITY_8K) & 
        (currentCapacity != CAPACITY_16K) & 
        (currentCapacity != CAPACITY_32K)) {
        currentCapacity = CAPACITY_8K;
    }
    
    // Verify max capacity
    if (currentCapacity > CAPACITY_32K) {
        currentCapacity = CAPACITY_32K;
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
        
        // Print file name
        print(&buffer[1], FILE_NAME_LENGTH + 1);
        
        printSpace(1);
        
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
            
            uint8_t directoryAttribute[] = "<dir>";
            print( &directoryAttribute[0], sizeof(directoryAttribute) );
            
        }
        
        // File
        if (fileAttributes[0] != 'd') {
            
            uint8_t nameOffsetLength = 6 - len;
            
            if (nameOffsetLength > 5) 
                nameOffsetLength = 5;
            
            printSpace(nameOffsetLength);
            
            print( integerString, len);
            
            print( &fileAttributes[0], 4 );
            
        }
        
        printLn();
        
        continue;
    }
    
    return;
}


uint8_t fsRepairDevice(void) {
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    
    uint8_t buffer[SECTOR_SIZE];
    
    // Get header sector
    uint32_t currentDevice = fsGetCurrentDevice();
    
    for (uint8_t i=0; i < SECTOR_SIZE; i++) 
        bus_read_byte(&bus, currentDevice + i, &buffer[i]);
    
    // Check header byte
    if (buffer[0] != 0x13) {
        
        uint8_t headerByte = 0x13;
        
        // Attempt to fix the header byte
        bus_write_byte_eeprom(currentDevice, headerByte);
        
    }
    
    // Check hardware name
    if ((buffer[1] != 'f') | (buffer[2] != 's')) {
        
        uint8_t deviceName[2] = {'f', 's'};
        
        // Attempt to fix the header byte
        bus_write_byte_eeprom(currentDevice + 1, deviceName[0]);
        bus_write_byte_eeprom(currentDevice + 2, deviceName[1]);
        
        
    }
    
    //union Pointer sizePointer;
    
    // Get device capacity
    //for (uint8_t i=0; i < 4; i++) 
    //    sizePointer.byte_t[i] = buffer[i + DEVICE_CAPACITY_OFFSET];
    
    return 1;
}



