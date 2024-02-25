#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

// Included drivers
#include <drivers/display/LiquidCrystalDisplayController/main.h>
#include <drivers/keyboard/ps2/main.h>

#include <kernel/console.h>

extern struct DisplayDeviceDriver* displayDriver;

void functionTest(void) {
    
    displayDriver->write( CLEAR_FRAME_BUFFER, 0x01 );
    
    _delay_ms(100);
    
    ConsoleSetCursor(0, 0);
    
    return;
}



int main(void) {
    
    // Zero the system bus
    bus_address_zero();
    bus_control_zero();
    
    // Allow board some time to stabilize
    _delay_ms(700);
	
	
	// Initiate drivers here
	
	initiateDisplayDriver();
	initiatePS2Driver();
	
	
	
	// Initiate the kernel
	
	InitiateDeviceTable();
    
    
    
    
    consoleInitiate();
    
    uint8_t commandName[] = "cls";
    
    ConsoleCommandRegister(commandName, sizeof(commandName), functionTest);
    
    while(1) {
        
        consoleUpdate();
        
    }
    
    
    return 0;
}
