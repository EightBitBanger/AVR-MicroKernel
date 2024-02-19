#include <avr/io.h>
#include <util/delay.h>

#include <kernel/bus.h>
#include <kernel/device.h>
#include <kernel/kernel.h>

#include <drivers/display/LiquidCrystalDisplayController/main.h>

int main(void) {
    
    bus_address_zero();
    bus_control_zero();
    
    // Allow board some time to stabilize
    _delay_ms(1000);
	
	initiateDisplayDriver();
	
    kernel_initiate();
    
	
	
	
	
	
	// Print off plugged in peripheral cards
	
	unsigned int index=0;
	
	for (unsigned int d=0; d < NUMBER_OF_PERIPHERALS; d++) {
        
        struct Device* device = GetDevice(d);
        
        if (is_letter( &device->device_name[1] ) == 0) 
            continue;
        
        displayDriver->print( index, 0, &device->device_name[0], DEVICE_NAME_LENGTH );
        
        
        index++;
    }
    
	_delay_ms( 300 );
    
    
    //displayDriver->shiftFrameDown();
	
	
	
	
	
    
    //bus_address_zero();
    bus_control_zero();
    
    
    
    PORTD = 0x00;
    
    PORTD |= (1 << PD4);  // High
    //PORTD |= (1 << PD5);  // High
    
    
    while(1) {
        
        continue;
    }
    
    return 0;
}
