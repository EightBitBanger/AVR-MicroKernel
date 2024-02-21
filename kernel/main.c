#include <avr/io.h>
#include <util/delay.h>

#include <kernel/kernel.h>

#include <drivers/display/LiquidCrystalDisplayController/main.h>



//
// Kernel entry point

int main(void) {
    
    bus_address_zero();
    bus_control_zero();
    
    // Allow board some time to stabilize
    _delay_ms(1000);
	
	// Initiate drivers here
	
	initiateDisplayDriver();
	
	
	
	
	
	
	
	
	
	RegisterDriver( (void*)displayDriver );
	
	
	
	
	// Initiate the kernel
	InitiateDeviceTable();
    
    
    
	
	
	
	// Print off names of attached peripheral devices
	
	displayDriver->cursorSetBlinkRate(0);
	
	for (unsigned int d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        struct Device* device = GetHardwareDevice(d);
        
        displayDriver->print( d, 0, &device->device_name[1], DEVICE_NAME_LENGTH - 1 );
        
    }
    
	_delay_ms( 300 );
    
    
    //displayDriver->shiftFrameDown();
	
	
	
	
	
    
    //bus_address_zero();
    bus_control_zero();
    
    
    
    PORTD = 0x00;
    
    //PORTD |= (1 << PD4);  // High
    PORTD |= (1 << PD5);  // High
    
    
    while(1) {
        
        continue;
    }
    
    return 0;
}
