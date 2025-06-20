<<<<<<< HEAD
/*
 
 Driver file layout
    
    Driver file ID         2 bytes 'KD'
    
    Driver name           10 Bytes
    
    Marker byte            1 byte '$'
    
    Device ID              1 byte
    
    Bus read waitstate     1 byte
    Bus write waitstate    1 byte
    
    Bus interface type     1 byte
      Type 0 - Default bus
      Type 1 - Memory bus
      Type 2 - IO bus
      Type 3 - Serial bus
    
    Hardware address       4 bytes
    
    
 Function blocks
    
    Number of blocks       4 bytes
    
    [address to block a]  [address to block b]  [address to block c]...
    
    [begin block a] ..assembly... [return]
    [begin block b] ..assembly... [return]
    [begin block c] ..assembly... [return]
    
*/

#ifndef __DRIVER_TABLE_
#define __DRIVER_TABLE_

#include <avr/io.h>

#include <kernel/bus/bus.h>

#include <kernel/list.h>
#include <kernel/device/device.h>

struct Driver {
    
    /// Device associated with this driver
    struct Device device;
    
    /// Is the driver currently linked to a hardware device
    uint8_t is_linked;
    
    /// Low level IO interface
    void (*read)(uint32_t address, uint8_t* buffer);
    void (*write)(uint32_t address, uint8_t buffer);
    
};


int8_t LoadLibrary(uint8_t* filename, uint8_t filenameLength);

struct Driver* GetDriverByName(uint8_t* name, uint8_t nameLength);

struct Driver* GetDriverByIndex(uint8_t index);

uint32_t RegisterDriver(void* deviceDriverPtr);

uint32_t GetNumberOfDrivers(void);

#endif
=======
/*
 
 Driver file layout
    
    Driver file ID         2 bytes 'KD'
    
    Driver name           10 Bytes
    
    Marker byte            1 byte '$'
    
    Device ID              1 byte
    
    Bus read waitstate     1 byte
    Bus write waitstate    1 byte
    
    Bus interface type     1 byte
      Type 0 - Default bus
      Type 1 - Memory bus
      Type 2 - IO bus
      Type 3 - Serial bus
    
    Hardware address       4 bytes
    
    
 Function blocks
    
    Number of blocks       4 bytes
    
    [address to block a]  [address to block b]  [address to block c]...
    
    [begin block a] ..assembly... [return]
    [begin block b] ..assembly... [return]
    [begin block c] ..assembly... [return]
    
*/

#ifndef __DRIVER_TABLE_
#define __DRIVER_TABLE_

#include <avr/io.h>

#include <kernel/bus/bus.h>

#include <kernel/list.h>
#include <kernel/device/device.h>

struct Driver {
    
    /// Device associated with this driver
    struct Device device;
    
    /// Is the driver currently linked to a hardware device
    uint8_t is_linked;
    
    /// Low level IO interface
    void (*read)(uint32_t address, uint8_t* buffer);
    void (*write)(uint32_t address, uint8_t buffer);
    
};


int8_t LoadLibrary(uint8_t* filename, uint8_t filenameLength);

struct Driver* GetDriverByName(uint8_t* name, uint8_t nameLength);

struct Driver* GetDriverByIndex(uint8_t index);

uint32_t RegisterDriver(void* deviceDriverPtr);

uint32_t GetNumberOfDrivers(void);

#endif
>>>>>>> aafa4c74e0c7a446ecc0e78b8d5dac3d7bb09fcf
