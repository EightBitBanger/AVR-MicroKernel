#include <avr/io.h>

#include <kernel/kernel.h>

#include <kernel/command/device/device.h>

void functionDevice(uint8_t* param, uint8_t param_length) {
    
    uint8_t msgPressAnyKey[]   = "Press any key...";
    
    uint8_t lineCount = 0;
    
    //
    // Display linked drivers
    
    if ((param[0] == '-') & 
        (param[1] == 'd') & 
        (param[2] == 'r') & 
        (param[3] == 'v')) {
        
        uint8_t numberOfDrivers = GetNumberOfDrivers();
        
        for (uint8_t i=0; i < numberOfDrivers; i++) {
            
            struct Driver* drv = GetDriverByIndex( i );
            
            if ((param[4] == ' ') & 
                (param[5] == 'l') & 
                (param[6] == 'n') & 
                (param[7] == 'k')) {
                
                if (drv->is_linked == 0) 
                    continue;
            }
            
            for (uint8_t i=0; i < DEVICE_NAME_LENGTH; i++) {
                if (drv->device.device_name[i] != 0) {
                    printChar(drv->device.device_name[i]);
                } else {
                    printSpace(1);
                }
            }
            
            uint8_t addr = ((drv->device.hardware_address - PERIPHERAL_ADDRESS_BEGIN) + 0x10000) / 0x10000;
            
            if (drv->device.hardware_address != 0) {
                
                uint8_t deviceAddr[] = "0x00000";
                
                deviceAddr[2] += addr;
                print(&deviceAddr[0], sizeof(deviceAddr));
                
            }
            
            printLn();
            
            if (lineCount > 1) {
                
                print(msgPressAnyKey, sizeof(msgPressAnyKey));
                
                ConsoleSetCursorPosition( sizeof(msgPressAnyKey) - 1 );
                
                uint8_t keypress = ConsoleWait();
                
                ConsoleSetCursorPosition(0);
                
                for (uint8_t a=0; a < sizeof(msgPressAnyKey); a++) 
                    printChar(' ');
                
                ConsoleSetCursorPosition(0);
                
                lineCount = 0;
                
                if (keypress == 'c') 
                    return;
                
                continue;
            }
            
            lineCount++;
        }
        
        return;
    }
    
    //
    // Usage
    
    uint8_t msgHelpA[] = "-drv";
    uint8_t msgHelpB[] = "-drv lnk";
    print(msgHelpA, sizeof(msgHelpA));
    printLn();
    print(msgHelpB, sizeof(msgHelpB));
    printLn();
    
    return;
}



void registerCommandDevice(void) {
    
    uint8_t deviceCommandName[] = "device";
    
    ConsoleRegisterCommand(deviceCommandName, sizeof(deviceCommandName), functionDevice);
    
    return;
}
