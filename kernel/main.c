#include <avr/io.h>
//#include <stdlib.h>

#include <util/delay.h>

#include <kernel/kernel.h>

// Included console commands
#include <kernel/commands/list.h>
#include <kernel/commands/cls.h>

#include <kernel/commands/fs/cap.h>
#include <kernel/commands/fs/cd.h>
#include <kernel/commands/fs/dir.h>
#include <kernel/commands/fs/mk.h>
#include <kernel/commands/fs/rm.h>
#include <kernel/commands/fs/repair.h>
#include <kernel/commands/fs/format.h>


// Included drivers
#include <drivers/display/LiquidCrystalDisplayController/main.h>
#include <drivers/keyboard/ps2/main.h>

#include <kernel/console.h>



int main(void) {
    
    // Zero the system bus
    bus_address_zero();
    bus_control_zero();
    
    // Allow board some time to stabilize
    _delay_ms(1500);
	
	
	// Initiate drivers here
	
	initiateDisplayDriver();
	initiatePS2Driver();
	
	
	
	// Initiate the kernel
	
	InitiateDeviceTable();
    
    
    // Initiate console commands
    consoleInitiate();
    
    registerCommandList();
    registerCommandCLS();
    
    registerCommandCAP();
    registerCommandCD();
    registerCommandDIR();
    registerCommandMK();
    registerCommandRM();
    registerCommandRepair();
    registerCommandFormat();
    
    
    // Allocate external memory
    struct Bus bus;
	
	bus.read_waitstate  = 4;
	bus.write_waitstate = 4;
	
	uint32_t total = 0;
	uint8_t counter = 0;
	
	for (uint32_t address=0x00000; address < 0x40000; address++) {
        
        bus_write_byte(&bus, address, 0xff);
        
        uint8_t buffer=0;
        bus_read_byte(&bus, address, &buffer);
        
        if (buffer != 0xff) {
            break;
        }
        
        total++;
        
        ConsoleSetCursor(0, 0);
        
        counter++;
        if (counter < 10) 
            continue;
        counter = 0;
        
        uint8_t totalString[10];
        uint8_t place = int_to_string(total, totalString);
        print(totalString, place + 1);
        
        continue;
	}
	
    uint8_t totalString[10];
    uint8_t place = int_to_string(total, totalString);
    
    ConsoleSetCursor(0, 0);
    print(totalString, place + 1);
    
    uint8_t byteFreeString[] = "bytes free";
    print(byteFreeString, sizeof(byteFreeString));
    
    printLn();
    
    
    // Set the root directory to C
    fsSetCurrentDevice( 2 );
    uint8_t prompt[] = {'C', '>'};
    ConsoleSetPrompt( prompt, sizeof(prompt) + 1 );
    
    _delay_ms(100);
    
    printPrompt();
    
    while(1) {
        
        consoleUpdate();
        
    }
    
    
    return 0;
}
