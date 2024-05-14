#include <avr/io.h>

#include <kernel/delay.h>
#include <kernel/kernel.h>

#include <kernel/commands/fs/format.h>

void functionFORMAT(uint8_t* param, uint8_t param_length) {
    
    uint32_t deviceCapacity = 0;
    
    if  (param[0] == '8') deviceCapacity = CAPACITY_8K;
    if ((param[0] == '1') & (param[1] == '6')) deviceCapacity = CAPACITY_16K;
    if ((param[0] == '3') & (param[1] == '2')) deviceCapacity = CAPACITY_32K;
    
    uint32_t deviceCapacityCurrent=0;
    if (deviceCapacity == 0) {
        deviceCapacityCurrent = fsGetDeviceCapacity() / 8;
    } else {
        deviceCapacityCurrent = deviceCapacity / 8;
    }
    
    uint8_t deviceCapacityMsg[] = "k bytes";
    uint8_t deviceCapacityAmount[10];
    
    uint8_t place = int_to_string(deviceCapacityCurrent, &deviceCapacityAmount[0]);
    
    print(deviceCapacityAmount, place + 1);
    print(deviceCapacityMsg, sizeof(deviceCapacityMsg));
    
    printLn();
    
    uint8_t devicePressToContinue[] = "Press Y to format";
    print(devicePressToContinue, sizeof(devicePressToContinue));
    printLn();
    
    if (consoleWait('y') == 0) {
        return;
    }
    
    struct Bus bus;
    bus.read_waitstate  = 4;
    bus.write_waitstate = 5;
    
    uint32_t deviceCapacityBytes = (deviceCapacityCurrent * 1024);
    
    uint32_t currentDevice = fsGetCurrentDevice();
    
    uint32_t cyclesPerPercent = (deviceCapacityCurrent * 1024) / 100;
    
    uint32_t sector = 0;
    
    ConsoleCursorDisable();
    
    uint16_t percentage = 0;
    uint8_t percentageString[10];
    
    uint8_t percentageSymbole[1];
    percentageSymbole[0] = '%';
    
    uint8_t sectorCounter = 0;
    
    // Arbitrary read to trigger EEPROM cache flush
    uint8_t dummy;
    bus_read_byte( &bus, 0, &dummy );
    _delay_ms(10);
    
    uint8_t pageCounter = 0;
    
    for (uint16_t i=0; i <= 1000; i++) {
        
        ConsoleSetCursorPosition(0);
        
        uint8_t place = int_to_string(percentage, &percentageString[0]);
        
        print(percentageString, place + 1);
        print(percentageSymbole, 2);
        
        percentage += 1;
        
        for (uint32_t c=0; c < cyclesPerPercent; c++) {
            
            sector++;
            
            if (sectorCounter < (SECTOR_SIZE - 1)) {
                bus_write_byte( &bus, currentDevice + sector, ' ');
                sectorCounter++;
            } else {
                bus_write_byte( &bus, currentDevice + sector, 0x00);
                sectorCounter = 0;
            }
            
            pageCounter++;
            if (pageCounter > 30) {
                pageCounter = 0;
                bus_read_byte( &bus, 0, &dummy );
                _delay_ms(10);
            }
            
            if (sector < deviceCapacityBytes) 
                continue;
            
            i = 1000;
            
            break;
        }
        
        continue;
    }
    
    // Finish as 100%
    ConsoleSetCursorPosition(0);
    
    uint8_t oneHundredPercentMsg[] = {'1', '0', '0', '%'};
    print( oneHundredPercentMsg, sizeof(oneHundredPercentMsg) );
    
    _delay_ms(10);
    
    // Initiate first sector
    bus_write_byte_eeprom( &bus, currentDevice    , 0x13 );
    bus_write_byte_eeprom( &bus, currentDevice + 1, 'f' );
    bus_write_byte_eeprom( &bus, currentDevice + 2, 's' );
    
    // Device total capacity
    union Pointer deviceSize;
    
    deviceSize.address = deviceCapacityBytes;
    
    for (uint8_t i=0; i < 4; i++) 
        bus_write_byte_eeprom( &bus, currentDevice + DEVICE_CAPACITY_OFFSET + i, deviceSize.byte_t[i] );
    
    ConsoleCursorEnable();
    
    printLn();
    
    return;
}

void registerCommandFormat(void) {
    
    uint8_t formatCommandName[] = "format";
    
    ConsoleRegisterCommand(formatCommandName, sizeof(formatCommandName), functionFORMAT);
    
    return;
}