#include <avr/io.h>
//#include <stdlib.h>

#include <util/delay.h>

#include <kernel/kernel.h>

// Included console commands
#include <kernel/commands/list.h>
#include <kernel/commands/repair.h>
#include <kernel/commands/cd.h>
#include <kernel/commands/cls.h>
#include <kernel/commands/dir.h>
#include <kernel/commands/cap.h>
#include <kernel/commands/mk.h>


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
    registerCommandRepair();
    
    registerCommandCD();
    registerCommandCLS();
    registerCommandDIR();
    registerCommandCAP();
    registerCommandMK();
    
    
    
    _delay_ms(100);
    
    printPrompt();
    
    while(1) {
        
        consoleUpdate();
        
    }
    
    
    return 0;
}
